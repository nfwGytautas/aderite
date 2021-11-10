#include "PhysicsActor.hpp"

#include <PxActor.h>
#include <PxScene.h>

#include "aderite/Aderite.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/PhysicsEventList.hpp"
#include "aderite/scene/Transform.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace physics {

PhysicsActor::~PhysicsActor() {
    for (Collider* collider : m_colliders) {
        delete collider;
    }

    if (p_actor != nullptr) {
        physx::PxScene* scene = p_actor->getScene();
        if (scene != nullptr) {
            scene->removeActor(*p_actor);
        }

        p_actor->release();
        p_actor = nullptr;
    }
}

void PhysicsActor::addCollider(physics::Collider* collider) {
    collider->setActor(this->p_actor);
    m_colliders.push_back(collider);
}

void PhysicsActor::removeCollider(physics::Collider* collider) {
    auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);

    if (it == m_colliders.end()) {
        return;
    }

    delete *it;
    m_colliders.erase(it);
}

void PhysicsActor::transferColliders(PhysicsActor* actor) {
    ADERITE_DYNAMIC_ASSERT(actor != nullptr, "Transfering colliders to nullptr actor");

    for (Collider* collider : m_colliders) {
        actor->addCollider(collider);
    }

    // Ownership was transfered
    m_colliders.clear();
}

void PhysicsActor::setEntity(scene::Entity* entity) {
    m_entity = entity;
}

void PhysicsActor::moveActor(const glm::vec3& position) {
    physx::PxTransform pxt = p_actor->getGlobalPose();
    pxt.p.x = position.x;
    pxt.p.y = position.y;
    pxt.p.z = position.z;

    p_actor->setGlobalPose(pxt);

    //// Collision group
    // physx::PxSetGroup(*actor, 0);
}

void PhysicsActor::rotateActor(const glm::quat& rotation) {
    physx::PxTransform pxt = p_actor->getGlobalPose();
    pxt.q.x = rotation.x;
    pxt.q.y = rotation.y;
    pxt.q.z = rotation.z;
    pxt.q.w = rotation.w;

    p_actor->setGlobalPose(pxt);
}

void PhysicsActor::sync() const {
    physx::PxTransform pxt = p_actor->getGlobalPose();
    scene::Transform* transform = m_entity->getTransform();

    transform->position() = {pxt.p.x, pxt.p.y, pxt.p.z};
    transform->rotation() = {pxt.q.w, pxt.q.x, pxt.q.y, pxt.q.z};

    // Set collider scales
    for (Collider* collider : m_colliders) {
        collider->setScale(transform->scale());
    }
}

void PhysicsActor::onTriggerEnter(PhysicsActor* trigger) {
    ::aderite::Engine::getPhysicsController()->getEventList()->registerEvent(TriggerEvent {trigger, this, true});
}

void PhysicsActor::onTriggerLeave(PhysicsActor* trigger) {
    ::aderite::Engine::getPhysicsController()->getEventList()->registerEvent(TriggerEvent {trigger, this, false});
}

void PhysicsActor::onCollisionEnter(PhysicsActor* collision) {
    ::aderite::Engine::getPhysicsController()->getEventList()->registerEvent(CollisionEvent {collision, this, true});
}

void PhysicsActor::onCollisionLeave(PhysicsActor* collision) {
    ::aderite::Engine::getPhysicsController()->getEventList()->registerEvent(CollisionEvent {collision, this, false});
}

reflection::Type aderite::physics::PhysicsActor::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::PHYSICS_ACTOR);
}
bool aderite::physics::PhysicsActor::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Colliders
    emitter << YAML::Key << "Colliders";
    emitter << YAML::BeginSeq;
    for (physics::Collider* c : m_colliders) {
        serializer->writeUntrackedType(emitter, c);
    }
    emitter << YAML::EndSeq;

    return true;
}
bool aderite::physics::PhysicsActor::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Colliders
    for (auto colliderEntry : data["Colliders"]) {
        YAML::Node& colliderNode = colliderEntry;
        physics::Collider* collider = static_cast<physics::Collider*>(serializer->parseUntrackedType(colliderEntry));
        this->addCollider(collider);
    }

    return true;
}

} // namespace physics
} // namespace aderite
