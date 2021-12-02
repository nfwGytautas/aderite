#include "Geometry.hpp"

#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Random.hpp"

namespace aderite {
namespace physics {

Geometry::Geometry() {}

Geometry::~Geometry() {
    ADERITE_DYNAMIC_ASSERT(p_shape->getActor() == nullptr, "Freed Geometry outside of BasePhysicsActor, because actor is still set");
    p_shape->release();
}

void Geometry::onPhysicsEventEnter(const Geometry* other) const {}

void Geometry::onPhysicsEventLeave(const Geometry* other) const {}

} // namespace physics
} // namespace aderite
