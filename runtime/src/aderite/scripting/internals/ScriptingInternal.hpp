#pragma once

#ifndef ADERITE_SCRIPTING_INTERNALS
#define ADERITE_SCRIPTING_INTERNALS
#else
#error "Multiple ScriptingInternal.hpp include"
#endif

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/ScriptedBehavior.hpp"

namespace internal_ {

namespace behavior {
MonoObject* GetGameObject(aderite::scripting::ScriptedBehavior* behavior) {
    return behavior->getGameObject()->getScriptInstance();
}

void linkBehavior() {
    mono_add_internal_call("Aderite.ScriptedBehavior::__GetGameObject(intptr)", reinterpret_cast<void*>(GetGameObject));
}
} // namespace behavior

} // namespace internal_

void linkScripting() {
    // ScriptedBehavior
    internal_::behavior::linkBehavior();
}
