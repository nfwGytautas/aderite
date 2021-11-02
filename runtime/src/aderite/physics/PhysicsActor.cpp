#include "PhysicsActor.hpp"

#include <PxActor.h>
#include <PxScene.h>

#include "aderite/physics/Collider.hpp"

namespace aderite {
namespace physics {

PhysicsActor::~PhysicsActor() {
    this->detachShapes();

    if (p_actor != nullptr) {
        physx::PxScene* scene = p_actor->getScene();
        if (scene != nullptr) {
            scene->removeActor(*p_actor);
        }

        p_actor->release();
        p_actor = nullptr;
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

void PhysicsActor::sync() {
    physx::PxTransform pxt = p_actor->getGlobalPose();

    scene::TransformComponent& transform = m_entity.getComponent<scene::TransformComponent>();
    transform.Position = {pxt.p.x, pxt.p.y, pxt.p.z};
    transform.Rotation = {pxt.q.w, pxt.q.x, pxt.q.y, pxt.q.z};
}

void PhysicsActor::detachShapes() {
    size_t shapeCount = p_actor->getNbShapes();
    std::vector<physx::PxShape*> shapes;
    shapes.resize(shapeCount);
    p_actor->getShapes(shapes.data(), shapeCount);
    for (physx::PxShape* shape : shapes) {
        p_actor->detachShape(*shape);
    }
}

void PhysicsActor::onTriggerEnter(PhysicsActor* trigger) {
    this->ensureEventComponent().TriggerEnter.push_back(trigger->m_entity);
}

void PhysicsActor::onTriggerLeave(PhysicsActor* trigger) {
    this->ensureEventComponent().TriggerLeave.push_back(trigger->m_entity);
}

void PhysicsActor::onCollisionEnter(PhysicsActor* collision) {
    this->ensureEventComponent().CollisionEnter.push_back(collision->m_entity);
}

void PhysicsActor::onCollisionLeave(PhysicsActor* collision) {
    this->ensureEventComponent().CollisionLeave.push_back(collision->m_entity);
}

scene::PhysicsCallbackComponent& PhysicsActor::ensureEventComponent() {
    if (!m_entity.hasComponent<scene::PhysicsCallbackComponent>()) {
        return m_entity.addComponent<scene::PhysicsCallbackComponent>();
    }

    return m_entity.getComponent<scene::PhysicsCallbackComponent>();
}

} // namespace physics
} // namespace aderite
