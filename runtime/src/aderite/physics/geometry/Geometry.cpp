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

} // namespace physics
} // namespace aderite
