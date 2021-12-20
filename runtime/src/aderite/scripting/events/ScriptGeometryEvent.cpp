#include "ScriptGeometryEvent.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/physics/geometry/Geometry.hpp"
#include "aderite/scripting/ScriptClass.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace aderite {
namespace scripting {

void ScriptGeometryEvent::call(physics::Geometry* geometry) {
    m_event(p_script->getInstance(), ::aderite::Engine::getScriptManager()->createInstance(geometry));
}

ScriptEventType ScriptGeometryEvent::getEventType() const {
    return ScriptEventType::GEOMETRY;
}

ScriptGeometryEvent::ScriptGeometryEvent(ScriptClass* script, MonoMethod* method) : ScriptEvent(script, method) {
    m_event = ThunkedMethod<void, MonoObject*>(p_method);
}

} // namespace scripting
} // namespace aderite
