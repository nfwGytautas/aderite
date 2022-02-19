#include "Geometry.hpp"

#include <PxRigidActor.h>

namespace aderite {
namespace physics {

Geometry::Geometry() {}

Geometry::~Geometry() {
    if (p_shape->getActor() != nullptr) {
        p_shape->getActor()->detachShape(*p_shape);
    }

    p_shape->userData = nullptr;
    p_shape->release();
}

PhysXActor* Geometry::getActor() const {
    physx::PxRigidActor* actor = p_shape->getActor();
    if (actor != nullptr) {
        return static_cast<PhysXActor*>(actor->userData);
    }

    return nullptr;
}

physx::PxShape* Geometry::getShape() const {
    return p_shape;
}

bool Geometry::isTrigger() const {
    return p_shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE;
}

void Geometry::setTrigger(bool value) {
    // Reset
    p_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
    p_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);

    // Set
    p_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !value);
    p_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, value);
}

} // namespace physics
} // namespace aderite
