#pragma once

#include <string>

#include <mono/jit/jit.h>

#include "aderite/scripting/Forward.hpp"
#include "aderite/scripting/ScriptEventType.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief A object used to wrap a script event callback
 */
class ScriptEvent {
public:
    virtual ~ScriptEvent() {}

    /**
     * @brief Returns the name of the event
     */
    const char* getName() const;

    /**
     * @brief Returns the full name of the event
     */
    std::string getFullName() const;

    /**
     * @brief Returns the type of the script event
     */
    virtual ScriptEventType getEventType() const = 0;

protected:
    ScriptEvent(ScriptClass* script, MonoMethod* method);
    friend class ScriptClass;

protected:
    ScriptClass* p_script = nullptr;
    MonoMethod* p_method = nullptr;
};

} // namespace scripting
} // namespace aderite
