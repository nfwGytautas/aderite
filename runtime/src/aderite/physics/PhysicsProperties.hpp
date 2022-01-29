#pragma once

#include "aderite/io/ISerializable.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Properties that can be applied to a physics actor
 */
struct PhysicsProperties {
    bool IsDynamic = false;

    // Dynamic only

    /**
     * @brief If true the actor is affected by gravity
     */
    bool HasGravity = false;

    /**
     * @brief Mass of the object
     */
    float Mass = 1.0f;
};

} // namespace physics
} // namespace aderite
