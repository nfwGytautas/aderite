#pragma once

#include "aderite/scripting/ScriptEvent.hpp"
#include "aderite/scripting/ThunkedMethod.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Script update event implementation
 */
class ScriptUpdateEvent : public ScriptEvent {
public:

    /**
     * @brief Invoke the event with the specified delta parameter
     * @param delta Delta of the frame
     */
    void call(float delta);

private:
    ScriptUpdateEvent(ScriptClass* script, MonoMethod* method);
    friend class ScriptClass;

private:
    ThunkedMethod<void, float> m_update;
};

} // namespace scripting
} // namespace aderite
