#include "ScriptDebug.hpp"

#include <mono/jit/jit.h>
#include "aderite/utility/Log.hpp"
#include "aderite/scripting/MonoUtils.hpp"

namespace aderite {
namespace scripting {

void Trace(MonoObject* message) {
	std::string msg = toString(message);
	LOG_TRACE(msg);
}

void Warn(MonoObject* message) {
	std::string msg = toString(message);
	LOG_WARN(msg);
}

void Error(MonoObject* message) {
	std::string msg = toString(message);
	LOG_ERROR(msg);
}

void logInternals() {
	mono_add_internal_call("Aderite.Log::__Trace(string)", Trace);
	mono_add_internal_call("Aderite.Log::__Warn(string)", Warn);
	mono_add_internal_call("Aderite.Log::__Error(string)", Error);
}

}
}
