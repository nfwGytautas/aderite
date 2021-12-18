#include "ScriptEvent.hpp"

#include "aderite/scripting/ScriptClass.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace scripting {

const char* ScriptEvent::getName() const {
    return mono_method_get_name(p_method);
}

std::string ScriptEvent::getFullName() const {
    return std::string(p_script->getName()) + "." + this->getName();
}

ScriptEvent::ScriptEvent(ScriptClass* script, MonoMethod* method) :
    p_script(script),
    p_method(method) {
    ADERITE_DYNAMIC_ASSERT(p_script != nullptr, "Null script passed to ScriptEvent");
    ADERITE_DYNAMIC_ASSERT(p_method != nullptr, "Null method passed to ScriptEvent");
}

} // namespace scripting
} // namespace aderite
