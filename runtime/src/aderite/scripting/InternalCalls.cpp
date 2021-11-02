#include "InternalCalls.hpp"

#include "aderite/scripting/internals/ScriptComponents.hpp"
#include "aderite/scripting/internals/ScriptDebug.hpp"
#include "aderite/scripting/internals/ScriptEntity.hpp"
#include "aderite/scripting/internals/ScriptInput.hpp"
#include "aderite/scripting/internals/ScriptPhysics.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

void linkInternals() {
    LOG_TRACE("Linking internals");

    logInternals();
    componentInternals();
    entityInternals();
    inputInternals();
    physicsInternals();

    LOG_TRACE("Internals linked");
}

} // namespace scripting
} // namespace aderite
