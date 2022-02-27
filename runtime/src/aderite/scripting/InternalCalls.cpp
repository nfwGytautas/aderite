#include "InternalCalls.hpp"

#include "aderite/scripting/internals/AssetInternal.hpp"
#include "aderite/scripting/internals/InputInternal.hpp"
#include "aderite/scripting/internals/SceneInternal.hpp"
#include "aderite/scripting/internals/ScriptingInternal.hpp"
#include "aderite/scripting/internals/UtilityInternal.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

void linkInternals() {
    LOG_TRACE("[Scripting] Linking internals");

    linkAsset();
    linkInput();
    linkScene();
    linkScripting();
    linkUtility();

    LOG_TRACE("[Scripting] Internals linked");
}

} // namespace scripting
} // namespace aderite
