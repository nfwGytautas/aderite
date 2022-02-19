#include "PhysXActor.hpp"

#include <physx/PxRigidDynamic.h>
#include <physx/PxRigidStatic.h>
#include <physx/PxScene.h>
#include <physx/PxShape.h>

#include "aderite/Aderite.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/geometry/Geometry.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace physics {

PhysXActor::PhysXActor(scene::GameObject* gObject) : m_gObject(gObject) {}

PhysXActor::~PhysXActor() {
    // Detach from scene
    this->freeActor();
}

void PhysXActor::update(float delta) {
    // Check if the actor needs to be converted
    this->createActor();

    // Sync properties
    if (m_isDynamic) {
        physx::PxRigidDynamic* dynamic = static_cast<physx::PxRigidDynamic*>(m_actor);
        dynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !m_properties.hasGravity());
        dynamic->setMass(m_properties.getMass());
    }

    if (m_properties.hasGeometryChanged()) {
        // Refresh geometry list
        for (Geometry* geom : m_properties.getAttachedGeometries()) {
            if (geom->getActor() == nullptr) {
                m_actor->attachShape(*geom->getShape());
            }
        }

        // Reset flag
        m_properties.resetGeometryChangedFlag();
    }

    // Sync position
    scene::TransformProvider* const transform = m_gObject->getTransform();
    if (transform != nullptr) {
        if (transform->wasModified()) {
            // Transform was modified sync with it

            // Regenerate shapes
            for (physics::Geometry* geometry : m_properties.getAttachedGeometries()) {
                geometry->applyScale(transform->getScale());
            }

            // Set spatial attributes
            physx::PxTransform pxt = m_actor->getGlobalPose();
            pxt.p = {transform->getPosition().x, transform->getPosition().y, transform->getPosition().z};
            pxt.q = {transform->getRotation().x, transform->getRotation().y, transform->getRotation().z, transform->getRotation().w};
            m_actor->setGlobalPose(pxt);
        } else {
            physx::PxTransform pxt = m_actor->getGlobalPose();
            transform->setPosition({pxt.p.x, pxt.p.y, pxt.p.z});
            transform->setRotation({pxt.q.w, pxt.q.x, pxt.q.y, pxt.q.z});
        }
    }
}

void PhysXActor::transferGeometry(physx::PxRigidActor* actor) {
    size_t count = m_actor->getNbShapes();
    std::vector<physx::PxShape*> shapes;
    shapes.resize(count);
    m_actor->getShapes(shapes.data(), count);

    for (physx::PxShape* shape : shapes) {
        m_actor->detachShape(*shape);
        actor->attachShape(*shape);
    }
}

void PhysXActor::freeActor() {
    physx::PxScene* scene = m_actor->getScene();

    if (scene != nullptr) {
        scene->removeActor(*m_actor);
    }
    m_actor->userData = nullptr;
    m_actor = nullptr;
}

void PhysXActor::createActor() {
    physx::PxRigidActor* newInstance = nullptr;

    if (m_properties.isDynamic() && (!m_isDynamic || m_actor == nullptr)) {
        // Convert to dynamic
        newInstance = ::aderite::Engine::getPhysicsController()->getPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0)));
        m_isDynamic = true;
    } else if (!m_properties.isDynamic() && (m_isDynamic || m_actor == nullptr)) {
        // Convert to static
        newInstance = ::aderite::Engine::getPhysicsController()->getPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(0)));
        m_isDynamic = false;
    }

    if (newInstance != nullptr) {
        newInstance->userData = this;

        // If this already has an actor, transfer colliders and position
        if (m_actor != nullptr) {
            this->transferGeometry(newInstance);
            newInstance->setGlobalPose(m_actor->getGlobalPose());
            this->freeActor();
        }

        m_actor = newInstance;
        m_gObject->getScene()->addActor(this);
    }
}

physx::PxRigidActor* PhysXActor::getActor() const {
    return m_actor;
}

PhysicsProperties& PhysXActor::getData() {
    return m_properties;
}

scene::GameObject* PhysXActor::getGameObject() const {
    return m_gObject;
}

} // namespace physics
} // namespace aderite
