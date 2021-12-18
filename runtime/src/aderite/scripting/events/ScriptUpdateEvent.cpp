#include "ScriptUpdateEvent.hpp"

#include "aderite/scripting/ScriptClass.hpp"

namespace aderite {
namespace scripting {

void ScriptUpdateEvent::call(float delta) {
    m_update(p_script->getInstance(), delta);
}

ScriptUpdateEvent::ScriptUpdateEvent(ScriptClass* script, MonoMethod* method) : ScriptEvent(script, method) {
    m_update = ThunkedMethod<void, float>(p_method);
}

} // namespace scripting
} // namespace aderite
