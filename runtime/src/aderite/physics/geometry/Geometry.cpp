#include "Geometry.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/events/ScriptGeometryEvent.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Random.hpp"

namespace aderite {
namespace physics {

Geometry::Geometry() {}

Geometry::~Geometry() {
    ADERITE_DYNAMIC_ASSERT(p_shape->getActor() == nullptr, "Freed Geometry outside of BasePhysicsActor, because actor is still set");
    p_shape->release();
}

void Geometry::onPhysicsEventEnter(Geometry* other) const {
    if (m_enter) {
        m_enter->call(other);
    }
}

void Geometry::onPhysicsEventLeave(Geometry* other) const {
    if (m_leave) {
        m_leave->call(other);
    }
}

void Geometry::setEnterEvent(scripting::ScriptGeometryEvent* e) {
    m_enter = e;
}

scripting::ScriptGeometryEvent* Geometry::getEnterEvent() {
    return m_enter;
}

void Geometry::setLeaveEvent(scripting::ScriptGeometryEvent* e) {
    m_leave = e;
}

scripting::ScriptGeometryEvent* Geometry::getLeaveEvent() {
    return m_leave;
}

bool Geometry::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (m_enter != nullptr) {
        emitter << YAML::Key << "OnEnterEvent" << YAML::Value << m_enter->getFullName();
    }

    if (m_leave != nullptr) {
        emitter << YAML::Key << "OnLeaveEvent" << YAML::Value << m_leave->getFullName();
    }

    return true;
}

bool Geometry::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& enterNode = data["OnEnterEvent"];
    if (enterNode && !enterNode.IsNull()) {
        m_enter = static_cast<scripting::ScriptGeometryEvent*>(
            ::aderite::Engine::getScriptManager()->getEventFromName(enterNode.as<std::string>()));
    }

    const YAML::Node& leaveNode = data["OnLeaveEvent"];
    if (leaveNode && !leaveNode.IsNull()) {
        m_leave = static_cast<scripting::ScriptGeometryEvent*>(
            ::aderite::Engine::getScriptManager()->getEventFromName(leaveNode.as<std::string>()));
    }

    return true;
}

} // namespace physics
} // namespace aderite
