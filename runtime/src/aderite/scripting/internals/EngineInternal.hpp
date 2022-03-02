#pragma once

#ifndef ADERITE_ENGINE_INTERNALS
#define ADERITE_ENGINE_INTERNALS
#else
#error "Multiple EngineInternal.hpp include"
#endif

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scripting/MonoUtils.hpp"

namespace internal_ {

namespace engine {

void StopPhysics() {
    ::aderite::Engine::get()->setState(::aderite::Engine::CurrentState::LOGIC);
}

MonoObject* FindGameObject(MonoObject* name) {
    std::string name_ = aderite::scripting::toString(name);
    for (auto& gObject : ::aderite::Engine::getSceneManager()->getCurrentScene()->getGameObjects()) {
        if (gObject->getName() == name_) {
            return gObject->getScriptInstance();
        }
    }

    return nullptr;
}

void linkEngine() {
    mono_add_internal_call("Aderite.Engine::__StopPhysics()", reinterpret_cast<void*>(StopPhysics));
    mono_add_internal_call("Aderite.Engine::__FindGameObject(string)", reinterpret_cast<void*>(FindGameObject));
}
} // namespace engine

} // namespace internal_

void linkEngine() {
    // Engine
    internal_::engine::linkEngine();
}
