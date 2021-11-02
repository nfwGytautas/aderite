#pragma once

#include "aderite/physics/Forward.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Component used to specify that this entity is a static physics object
 */
struct StaticActor {
    physics::StaticActor* Actor = nullptr;

    StaticActor() = default;
    StaticActor(const StaticActor&) = default;
};

/**
 * @brief Component used to specify that this entity is a dynamic physics object
 */
struct DynamicActor {
    physics::DynamicActor* Actor = nullptr;

    DynamicActor() = default;
    DynamicActor(const DynamicActor&) = default;
};

/**
 * @brief Helper component to transfer physics callback
 */
struct PhysicsCallbackComponent {
    std::vector<Entity> TriggerEnter;
    std::vector<Entity> TriggerLeave;

    std::vector<Entity> CollisionEnter;
    std::vector<Entity> CollisionLeave;

    PhysicsCallbackComponent() = default;
    PhysicsCallbackComponent(const PhysicsCallbackComponent&) = default;
};

} // namespace scene
} // namespace aderite
