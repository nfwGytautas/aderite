#include "PhysicsEventList.hpp"

namespace aderite {
namespace physics {

void PhysicsEventList::registerEvent(const TriggerEvent& te) {
    m_triggerEvents.push_back(te);
}

void PhysicsEventList::registerEvent(const CollisionEvent& ce) {
    m_collisionEvents.push_back(ce);
}

void PhysicsEventList::clear() {
    m_triggerEvents.clear();
    m_collisionEvents.clear();
}

} // namespace physics
}
