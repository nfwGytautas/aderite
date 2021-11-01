#pragma once

#include "aderite/physics/PhysicsActor.hpp"

namespace aderite {
namespace physics {

/**
 * @brief The class used to notify about physics events
 */
class EventNotifier final {
public:
    void onTriggerEnter(PhysicsActor* trigger) {}
    void onTriggerLeave(PhysicsActor* trigger) {}

    void onCollisionEnter(PhysicsActor* collision) {}
    void onCollisionLeave(PhysicsActor* collision) {}

private:
};

} // namespace physics
} // namespace aderite
