#include "Collider.hpp"

#include <PxRigidActor.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace physics {

Collider::Collider() {}

Collider::~Collider() {
    LOG_TRACE("[Physics] Destroying collider");
    if (p_shape != nullptr) {
        physx::PxRigidActor* prevActor = p_shape->getActor();
        if (prevActor != nullptr) {
            prevActor->detachShape(*p_shape);
        }

        p_shape->release();
    } else {
        LOG_WARN("[Physics] Collider with no shape, doing nothing");
    }
    LOG_INFO("[Physics] Collider destroyed");
}

void Collider::setScale(const glm::vec3& scale) {
    if (p_scale == scale) {
        // Scale didn't change
        return;
    }
    LOG_TRACE("[Physics] Changing {0:p} collider scale", static_cast<void*>(this));

    p_scale = scale;
    this->updateGeometry();
}

bool Collider::isTrigger() const {
    return p_shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE;
}

void Collider::setTrigger(bool value) {
    LOG_TRACE("[Physics] Changing {0:p} collider trigger flag to {1}", static_cast<void*>(this), value);

    // Reset
    p_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
    p_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);

    // Set
    p_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !value);
    p_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, value);
}

void Collider::setActor(physx::PxRigidActor* actor) {
    LOG_TRACE("[Physics] Changing {0:p} collider actor to {1:p}", static_cast<void*>(this), static_cast<void*>(actor));

    if (p_shape == nullptr) {
        this->createShape();
    }

    physx::PxRigidActor* prevActor = p_shape->getActor();
    if (prevActor != nullptr) {
        p_shape->acquireReference();
        prevActor->detachShape(*p_shape);
    }
    actor->attachShape(*p_shape);
}

bool Collider::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "IsTrigger" << YAML::Value << isTrigger();
    return true;
}

bool Collider::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (p_shape == nullptr) {
        this->createShape();
    }

    setTrigger(data["IsTrigger"].as<bool>());
    return true;
}

void Collider::updateGeometry() {
    LOG_TRACE("[Physics] Updating collider geometry");
    if (p_shape == nullptr) {
        this->createShape();
    } else {
        physx::PxGeometry* geometry = this->genGeometry();
        p_shape->setGeometry(*geometry);
        delete geometry;
    }
}

physics::PhysicsActor* Collider::getActor() const {
    if (p_shape == nullptr || p_shape->getActor() == nullptr) {
        return nullptr;
    }

    return static_cast<physics::PhysicsActor*>(p_shape->getActor()->userData);
}

void Collider::createShape() {
    LOG_TRACE("[Physics] Creating shape for {0:p} collider actor", static_cast<void*>(this));

    ADERITE_DYNAMIC_ASSERT(p_shape == nullptr, "Creating a shape for a collider that already has a shape object");

    physx::PxPhysics* physics = ::aderite::Engine::getPhysicsController()->getPhysics();
    physx::PxMaterial* defaultMaterial = ::aderite::Engine::getPhysicsController()->getDefaultMaterial();
    physx::PxShapeFlags baseFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE |
                                    physx::PxShapeFlag::eSIMULATION_SHAPE;

    physx::PxGeometry* geometry = this->genGeometry();

    p_shape = physics->createShape(*geometry, *defaultMaterial, true, baseFlags);

    if (p_shape == nullptr) {
        LOG_ERROR("[Physics] Failed to create {0:p} collider shape", static_cast<void*>(this));
    }

    delete geometry;
}

} // namespace physics
} // namespace aderite
