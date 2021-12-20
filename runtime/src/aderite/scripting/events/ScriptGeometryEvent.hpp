#pragma once

#include "aderite/physics/Forward.hpp"
#include "aderite/scripting/ScriptEvent.hpp"
#include "aderite/scripting/ThunkedMethod.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Script geometry event implementation
 */
class ScriptGeometryEvent : public ScriptEvent {
public:
    /**
     * @brief Invoke the event with the specified geometry
     * @param geometry Geometry that entered
     */
    void call(physics::Geometry* geometry);

    // Inherited from ScriptEvent
    ScriptEventType getEventType() const override;

private:
    ScriptGeometryEvent(ScriptClass* script, MonoMethod* method);
    friend class ScriptClass;

private:
    ThunkedMethod<void, MonoObject*> m_event;
};

} // namespace scripting
} // namespace aderite
