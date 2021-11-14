#include "PhysicsEventList.hpp"

namespace aderite {
namespace physics {

void PhysicsEventList::registerEvent(const TriggerEvent& te) {
    // Check if opposite already exists
    auto it = std::find_if(m_triggerEvents.begin(), m_triggerEvents.end(), [&te](const TriggerEvent& trigger) {
        return trigger.Enter == te.Enter && ((te.Actor == trigger.Actor && te.Trigger == trigger.Trigger) ||
                                             (te.Trigger == trigger.Actor && te.Actor == trigger.Trigger));
    });

    if (it == m_triggerEvents.end()) {
        m_triggerEvents.push_back(te);
    }
}

void PhysicsEventList::registerEvent(const CollisionEvent& ce) {
    // Check if opposite already exists
    auto it = std::find_if(m_collisionEvents.begin(), m_collisionEvents.end(), [&ce](const CollisionEvent& collision) {
        return ce.Start == ce.Start && ((ce.Actor1 == collision.Actor1 && ce.Actor2 == collision.Actor2) ||
                                        (ce.Actor2 == collision.Actor1 && ce.Actor1 == collision.Actor2));
    });

    if (it == m_collisionEvents.end()) {
        m_collisionEvents.push_back(ce);
    }
}

const std::vector<TriggerEvent>& PhysicsEventList::getTriggerEvents() const {
    return m_triggerEvents;
}

const std::vector<CollisionEvent>& PhysicsEventList::getCollisionEvents() const {
    return m_collisionEvents;
}

void PhysicsEventList::clear() {
    m_triggerEvents.clear();
    m_collisionEvents.clear();
}

} // namespace physics
} // namespace aderite
