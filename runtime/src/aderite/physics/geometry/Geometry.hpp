#pragma once

#include <string>

#include <glm/glm.hpp>
#include <physx/PxShape.h>

#include "aderite/io/SerializableObject.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Geometry is the interface class for geometry types for collider shapes
 */
class Geometry : public io::NamedSerializable {
public:
    Geometry();
    virtual ~Geometry();

    /**
     * @brief Function invoked by the physics scene when this geometry had a physics event happen to it, specifically a enter
     * @param other Geometry instance that was collided/entered
     */
    void onPhysicsEventEnter(const Geometry* other) const;

    /**
     * @brief Function invoked by the physics scene when this geometry had a physics event happen to it, specifically a leave
     * @param other Geometry instance that was collided/entered
     */
    void onPhysicsEventLeave(const Geometry* other) const;

    /**
     * @brief Apply scale to geometry
     * @param scale Scale to apply
     */
    virtual void applyScale(const glm::vec3& scale) = 0;

protected:
    // Needed to access p_shape
    friend class BasePhysicsActor;

    physx::PxShape* p_shape = nullptr;
};

} // namespace physics
} // namespace aderite
