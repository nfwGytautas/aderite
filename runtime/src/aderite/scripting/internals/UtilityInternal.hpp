#pragma once

#ifndef ADERITE_UTILITY_INTERNALS
#define ADERITE_UTILITY_INTERNALS
#else
#error "Multiple UtilityInternal.hpp include"
#endif

#include <mono/jit/jit.h>

#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/utility/Log.hpp"

namespace internal_ {

namespace log {
void Trace(MonoObject* message) {
    std::string msg = aderite::scripting::toString(message);
    LOG_TRACE("[Game code] {0}", msg);
}

void Warn(MonoObject* message) {
    std::string msg = aderite::scripting::toString(message);
    LOG_WARN("[Game code] {0}", msg);
}

void Error(MonoObject* message) {
    std::string msg = aderite::scripting::toString(message);
    LOG_ERROR("[Game code] {0}", msg);
}

void linkLog() {
    mono_add_internal_call("Aderite.Log::__Trace(string)", reinterpret_cast<void*>(Trace));
    mono_add_internal_call("Aderite.Log::__Warn(string)", reinterpret_cast<void*>(Warn));
    mono_add_internal_call("Aderite.Log::__Error(string)", reinterpret_cast<void*>(Error));
}
} // namespace log

} // namespace internal_

void linkUtility() {
    // Log
    internal_::log::linkLog();
}
