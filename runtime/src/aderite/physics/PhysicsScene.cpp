#include "PhysicsScene.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysXActor.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/PhysicsEventList.hpp"
#include "aderite/physics/PhysicsSceneQuery.hpp"
#include "aderite/physics/geometry/Geometry.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace physics {

PhysicsScene::PhysicsScene() {
    LOG_TRACE("[Physics] Creating physics scene");

    auto physics = ::aderite::Engine::getPhysicsController()->getPhysics();

    // TODO: Configure physics properties
    physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = ::aderite::Engine::getPhysicsController()->getDispatcher();
    sceneDesc.filterShader = physics::PhysicsController::filterShader;
    sceneDesc.simulationEventCallback = this;
    // sceneDesc.flags = physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    m_scene = physics->createScene(sceneDesc);
    m_scene->userData = this;
    ADERITE_DYNAMIC_ASSERT(m_scene != nullptr, "Failed to create a PhysX scene");

    // Create event list
    m_events = new PhysicsEventList();

    LOG_INFO("[Physics] Physics scene created");
}

PhysicsScene::~PhysicsScene() {
    LOG_TRACE("[Physics] Destroying physics scene");

    if (m_scene != nullptr) {
        m_scene->release();
    } else {
        LOG_WARN("[Physics] nullptr PhysX scene");
    }

    delete m_events;

    LOG_INFO("[Physics] Physics scene destroyed");
}

void PhysicsScene::simulate(float step) const {
    m_scene->simulate(step);
    m_scene->fetchResults(true);
}

void PhysicsScene::addActor(PhysXActor* actor) {
    m_scene->addActor(*actor->getActor());
}

void PhysicsScene::sendEvents() {
    // TODO: Think if only to send to trigger
    for (const TriggerEvent& te : m_events->getTriggerEvents()) {
        if (te.Enter) {
            te.Actor->getActor()->getGameObject()->onTriggerEnter(te);
            te.Trigger->getActor()->getGameObject()->onTriggerWasEntered(te);
        } else {
            te.Actor->getActor()->getGameObject()->onTriggerLeave(te);
            te.Trigger->getActor()->getGameObject()->onTriggerWasLeft(te);
        }
    }

    for (const CollisionEvent& ce : m_events->getCollisionEvents()) {
        if (ce.Start) {
            ce.Actor1->getActor()->getGameObject()->onCollisionEnter(ce);
            ce.Actor2->getActor()->getGameObject()->onCollisionEnter(ce);
        } else {
            ce.Actor1->getActor()->getGameObject()->onCollisionLeave(ce);
            ce.Actor2->getActor()->getGameObject()->onCollisionLeave(ce);
        }
    }

    // Clear old events
    m_events->clear();
}

bool PhysicsScene::raycastSingle(RaycastHit& result, const glm::vec3& from, const glm::vec3& direction, float maxDistance) {
    physx::PxRaycastBuffer hit;
    const bool hadHit = m_scene->raycast({from.x, from.y, from.z}, {direction.x, direction.y, direction.z}, maxDistance, hit);
    if (!hadHit) {
        // No hits
        return false;
    }

    // Fill result
    result.Actor = static_cast<PhysXActor*>(hit.block.actor->userData);
    result.Distance = hit.block.distance;

    return true;
}

void PhysicsScene::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {
    for (physx::PxU32 i = 0; i < nbPairs; i++) {
        const physx::PxContactPair& cp = pairs[i];

        physx::PxRigidActor* actor1 = pairHeader.actors[0];
        physx::PxRigidActor* actor2 = pairHeader.actors[1];

        Geometry* geom1 = static_cast<Geometry*>(cp.shapes[0]->userData);
        Geometry* geom2 = static_cast<Geometry*>(cp.shapes[1]->userData);

        if (geom1 == nullptr || geom2 == nullptr) {
            continue;
        }

        // Send notifications
        if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            m_events->registerEvent(CollisionEvent {geom1, geom2, true});
        } else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            m_events->registerEvent(CollisionEvent {geom1, geom2, false});
        }
    }
}

void PhysicsScene::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs) {
    for (physx::PxU32 i = 0; i < nbPairs; i++) {
        const physx::PxTriggerPair& cp = pairs[i];

        physx::PxRigidActor* actor = cp.otherActor;
        physx::PxRigidActor* trigger = cp.triggerActor;

        Geometry* actorGeom = static_cast<Geometry*>(cp.otherShape->userData);
        Geometry* triggerGeom = static_cast<Geometry*>(cp.triggerShape->userData);

        if (actorGeom == nullptr || triggerGeom == nullptr) {
            continue;
        }

        // Send notifications
        if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            m_events->registerEvent(TriggerEvent {triggerGeom, actorGeom, true});
        } else if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            m_events->registerEvent(TriggerEvent {triggerGeom, actorGeom, false});
        }
    }
}

bool PhysicsScene::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "PhysicsScene" << YAML::BeginMap;
    emitter << YAML::EndMap;

    return true;
}

bool PhysicsScene::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& physicsNode = data["PhysicsScene"];
    if (!physicsNode || physicsNode.IsNull()) {
        return false;
    }

    return true;
}

} // namespace physics
} // namespace aderite