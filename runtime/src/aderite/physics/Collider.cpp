#include "Collider.hpp"

#include <PxRigidActor.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace physics {

Collider::Collider(physx::PxGeometry* geometry) {
    physx::PxPhysics* physics = ::aderite::Engine::getPhysicsController()->getPhysics();
    physx::PxMaterial* defaultMaterial = ::aderite::Engine::getPhysicsController()->getDefaultMaterial();
    physx::PxShapeFlags baseFlags = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE |
                                    physx::PxShapeFlag::eSIMULATION_SHAPE;

    p_shape = physics->createShape(*geometry, *defaultMaterial, true, baseFlags);

    if (p_shape == nullptr) {
        LOG_ERROR("Failed to create collider shape");
    }

    delete geometry;
}

Collider::~Collider() {
    if (p_shape != nullptr) {
        physx::PxRigidActor* prevActor = p_shape->getActor();
        if (prevActor != nullptr) {
            prevActor->detachShape(*p_shape);
        }

        p_shape->release();
    }
}

bool Collider::isTrigger() const {
    return p_shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE;
}

void Collider::setTrigger(bool value) {
    // Reset
    p_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
    p_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);

    // Set
    p_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !value);
    p_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, value);
}

void Collider::setActor(physx::PxRigidActor* actor) {
    physx::PxRigidActor* prevActor = p_shape->getActor();
    if (prevActor != nullptr) {
        p_shape->acquireReference();
        prevActor->detachShape(*p_shape);
    }
    actor->attachShape(*p_shape);
}

bool Collider::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    emitter << YAML::Key << "IsTrigger" << YAML::Value << isTrigger();
    return true;
}

bool Collider::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    setTrigger(data["IsTrigger"].as<bool>());
    return true;
}

} // namespace physics
} // namespace aderite
