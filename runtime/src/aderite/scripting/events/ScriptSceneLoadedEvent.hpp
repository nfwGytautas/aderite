#pragma once

#include "aderite/scripting/ScriptEvent.hpp"
#include "aderite/scripting/ThunkedMethod.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Script scene loaded event implementation
 */
class ScriptSceneLoadedEvent : public ScriptEvent {
public:
    /**
     * @brief Invoke the event
     */
    void call();

    // Inherited from ScriptEvent
    ScriptEventType getEventType() const override;

private:
    ScriptSceneLoadedEvent(ScriptClass* script, MonoMethod* method);
    friend class ScriptClass;

private:
    ThunkedMethod<void> m_loaded;
};

} // namespace scripting
} // namespace aderite
