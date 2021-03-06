#pragma once

#include "aderite/physics/Forward.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Class for constructing a raycast query
 */
struct RaycastResult {
    /**
     * @brief Actor that was hit
     */
    PhysXActor* Actor = nullptr;

    /**
     * @brief Distance that the ray traveled
     */
    float Distance = 0.0f;
};

} // namespace physics
} // namespace aderite
