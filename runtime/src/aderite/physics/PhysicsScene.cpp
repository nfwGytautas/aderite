#include "PhysicsScene.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace physics {

PhysicsScene::PhysicsScene() {
    auto physics = ::aderite::Engine::getPhysicsController()->getPhysics();

    // TODO: Configure physics properties
    physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = ::aderite::Engine::getPhysicsController()->getDispatcher();
    sceneDesc.filterShader = physics::PhysicsController::filterShader;
    sceneDesc.simulationEventCallback = this;
    // sceneDesc.flags = physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    m_scene = physics->createScene(sceneDesc);
    ADERITE_DYNAMIC_ASSERT(m_scene != nullptr, "Failed to create a PhysX scene");
}

PhysicsScene::~PhysicsScene() {
    if (m_scene != nullptr) {
        size_t count = m_scene->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC | physx::PxActorTypeFlag::eRIGID_DYNAMIC);
        std::vector<physx::PxActor*> actors;
        actors.resize(count);

        m_scene->getActors(physx::PxActorTypeFlag::eRIGID_STATIC | physx::PxActorTypeFlag::eRIGID_DYNAMIC, actors.data(), count);

        for (physx::PxActor* actor : actors) {
            delete static_cast<physics::PhysicsActor*>(actor->userData);
        }

        m_scene->release();
    }
}

void PhysicsScene::simulate(float step) {
    m_scene->simulate(step);
    m_scene->fetchResults(true);
}

physics::PhysicsActor* PhysicsScene::createStaticBody(scene::Entity e, const scene::TransformComponent& transform) {
    physics::PhysicsActor* actor = new physics::StaticActor();

    // Initial position
    actor->moveActor(transform.Position);
    actor->rotateActor(transform.Rotation);

    actor->m_entity = e;
    this->m_scene->addActor(*actor->p_actor);

    return actor;
}

physics::PhysicsActor* PhysicsScene::createDynamicBody(scene::Entity e, const scene::TransformComponent& transform) {
    physics::PhysicsActor* actor = new physics::DynamicActor();

    // Initial position
    actor->moveActor(transform.Position);
    actor->rotateActor(transform.Rotation);

    actor->m_entity = e;
    this->m_scene->addActor(*actor->p_actor);

    return actor;
}

void PhysicsScene::detachActor(physics::PhysicsActor* actor) {
    delete actor;
}

void PhysicsScene::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {
    for (physx::PxU32 i = 0; i < nbPairs; i++) {
        const physx::PxContactPair& cp = pairs[i];

        physx::PxRigidActor* actor1 = pairHeader.actors[0];
        physx::PxRigidActor* actor2 = pairHeader.actors[1];

        PhysicsActor* e1 = static_cast<PhysicsActor*>(actor1->userData);
        PhysicsActor* e2 = static_cast<PhysicsActor*>(actor2->userData);

        // Send notifications
        if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            e1->onCollisionEnter(e2);
            e2->onCollisionEnter(e1);
        } else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            e1->onCollisionLeave(e2);
            e2->onCollisionLeave(e1);
        }
    }
}

void PhysicsScene::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs) {
    for (physx::PxU32 i = 0; i < nbPairs; i++) {
        const physx::PxTriggerPair& cp = pairs[i];

        physx::PxRigidActor* actor = cp.otherActor;
        physx::PxRigidActor* trigger = cp.triggerActor;

        PhysicsActor* paA = static_cast<PhysicsActor*>(actor->userData);
        PhysicsActor* paT = static_cast<PhysicsActor*>(trigger->userData);

        // Send notifications
        if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            paA->onTriggerEnter(paT);
            paT->onTriggerEnter(paA);
        } else if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            paA->onTriggerLeave(paT);
            paT->onTriggerLeave(paA);
        }
    }
}

} // namespace physics
} // namespace aderite