#include "PhysicsActor.hpp"

#include <PxActor.h>
#include <PxScene.h>

#include "aderite/physics/Collider.hpp"
#include "aderite/physics/ColliderList.hpp"
#include "aderite/scripting/ScriptList.hpp"

namespace aderite {
namespace physics {

PhysicsActor::~PhysicsActor() {
    if (m_colliders != nullptr) {
        m_colliders->detach();
    }

    if (p_actor != nullptr) {
        p_actor->release();
        p_actor = nullptr;
    }

    if (m_notifier != nullptr) {
        delete m_notifier;
    }
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

void PhysicsActor::sync(scene::TransformComponent& transform) const {
    physx::PxTransform pxt = p_actor->getGlobalPose();
    transform.Position = {pxt.p.x, pxt.p.y, pxt.p.z};
    transform.Rotation = {pxt.q.w, pxt.q.x, pxt.q.y, pxt.q.z};
}

void PhysicsActor::markForRemove() {
    m_marked = true;
}

bool PhysicsActor::isMarkedForRemoval() const {
    return m_marked;
}

void PhysicsActor::detach() {
    physx::PxScene* scene = p_actor->getScene();
    scene->removeActor(*p_actor);
}

void PhysicsActor::setNotifier(EventNotifier* notifier) {
    if (m_notifier != nullptr) {
        delete m_notifier;
    }

    m_notifier = notifier;
}

EventNotifier* PhysicsActor::getNotifier() const {
    return m_notifier;
}

void PhysicsActor::setColliders(ColliderList* colliders) {
    if (m_colliders != nullptr) {
        m_colliders->detach();
    }

    m_colliders = colliders;

    for (Collider* collider : *m_colliders) {
        if (collider->p_shape != nullptr) {
            // TODO: Handle error?
            p_actor->attachShape(*collider->p_shape);
        }
    }
}

void PhysicsActor::updateColliders() {
    this->setColliders(m_colliders);
}

} // namespace physics
} // namespace aderite
