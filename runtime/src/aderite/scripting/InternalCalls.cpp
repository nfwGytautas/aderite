#include "InternalCalls.hpp"

#include "aderite/scripting/internals/ScriptAudio.hpp"
#include "aderite/scripting/internals/ScriptCore.hpp"
#include "aderite/scripting/internals/ScriptPhysics.hpp"
#include "aderite/scripting/internals/ScriptScene.hpp"
#include "aderite/scripting/internals/ScriptUtility.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

void linkInternals() {
    LOG_TRACE("[Scripting] Linking internals");

    utilityInternals();
    physicsInternals();
    audioInternals();
    coreInternals();
    sceneInternals();

    LOG_TRACE("[Scripting] Internals linked");
}

} // namespace scripting
} // namespace aderite
