#include "InternalCalls.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/scripting/internals/ScriptDebug.hpp"
#include "aderite/scripting/internals/ScriptComponents.hpp"
#include "aderite/scripting/internals/ScriptEntity.hpp"
#include "aderite/scripting/internals/ScriptInput.hpp"

namespace aderite {
namespace scripting {

void linkInternals() {
	LOG_TRACE("Linking internals");

	logInternals();
	componentInternals();
	entityInternals();
	inputInternals();

	LOG_TRACE("Internals linked");
}

}
}
