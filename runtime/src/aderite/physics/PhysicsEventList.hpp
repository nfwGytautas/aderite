#pragma once

#include <vector>

#include "aderite/physics/Forward.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Trigger event
 */
struct TriggerEvent {
    const Geometry* Trigger = nullptr;
    const Geometry* Actor = nullptr;

    bool Enter = false;
};

/**
 * @brief Collision event
 */
struct CollisionEvent {
    const Geometry* Actor1 = nullptr;
    const Geometry* Actor2 = nullptr;

    bool Start = false;
};

/**
 * @brief A class containing physics events
 */
class PhysicsEventList final {
public:
    /**
     * @brief Register a trigger event to the list
     * @param te Trigger event
     */
    void registerEvent(const TriggerEvent& te);

    /**
     * @brief Register a collision event to the list
     * @param ce Collision event
     */
    void registerEvent(const CollisionEvent& ce);

    /**
     * @brief Returns the trigger events
     */
    const std::vector<TriggerEvent>& getTriggerEvents() const;

    /**
     * @brief Returns the collision events
     */
    const std::vector<CollisionEvent>& getCollisionEvents() const;

    /**
     * @brief Clears all events in the list
     */
    void clear();

private:
    std::vector<TriggerEvent> m_triggerEvents;
    std::vector<CollisionEvent> m_collisionEvents;
};

} // namespace physics
} // namespace aderite
