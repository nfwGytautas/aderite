#include "ScriptSceneLoadedEvent.hpp"

#include "aderite/scripting/ScriptClass.hpp"

namespace aderite {
namespace scripting {

void ScriptSceneLoadedEvent::call() {
    m_loaded(p_script->getInstance());
}

ScriptEventType ScriptSceneLoadedEvent::getEventType() const {
    return ScriptEventType::SCENE_LOADED;
}

ScriptSceneLoadedEvent::ScriptSceneLoadedEvent(ScriptClass* script, MonoMethod* method) : ScriptEvent(script, method) {
    m_loaded = ThunkedMethod<void>(p_method);
}

} // namespace scripting
} // namespace aderite
