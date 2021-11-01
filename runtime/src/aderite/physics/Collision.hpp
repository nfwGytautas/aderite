#pragma once

#include "aderite/physics/Forward.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Struct that is used to denote a physics collision between two actors
 */
struct Collision {
    /**
     * @brief The actor that was collided into
     */
    PhysicsActor* Into = nullptr;

    /**
     * @brief If true then the collision just happened, false means the collision just ended
    */
    bool Enter = true;
};

/**
 * @brief Struct that is used to denote a physics trigger
*/
struct Trigger {
    /**
     * @brief The actor that was triggered
    */
    PhysicsActor* Trigger = nullptr;

    /**
     * @brief If true then the trigger just happened, false means the trigger just ended
     */
    bool Enter = true;
};

} // namespace physics
} // namespace aderite
