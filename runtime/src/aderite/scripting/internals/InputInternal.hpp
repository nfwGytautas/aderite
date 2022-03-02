#pragma once

#ifndef ADERITE_INPUT_INTERNALS
#define ADERITE_INPUT_INTERNALS
#else
#error "Multiple InputInternal.hpp include"
#endif

#include <glm/glm.hpp>
#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace internal_ {

namespace input {

bool inputIsKeyDown(aderite::input::Key key) {
    return ::aderite::Engine::getInputManager()->isKeyPressed(key);
}

bool inputWasKeyReleased(aderite::input::Key key) {
    return ::aderite::Engine::getInputManager()->wasKeyReleased(key);
}

bool inputIsMouseButtonDown(aderite::input::MouseKey key) {
    return ::aderite::Engine::getInputManager()->isMouseKeyPressed(key);
}

glm::vec3 inputGetMousePosition() {
    glm::vec2 position = ::aderite::Engine::getInputManager()->getMousePosition();
    return {position.x, position.y, 0.0f};
}

glm::vec3 inputGetMouseDelta() {
    glm::vec2 delta = ::aderite::Engine::getInputManager()->getMouseDelta();
    return {delta.x, delta.y, 0.0f};
}

double inputGetScrollDelta() {
    return ::aderite::Engine::getInputManager()->getScrollDelta();
}

void linkInput() {
    mono_add_internal_call("Aderite.Input::__IsKeyDown(Aderite.Key)", reinterpret_cast<void*>(inputIsKeyDown));
    mono_add_internal_call("Aderite.Input::__WasKeyReleased(Aderite.Key)", reinterpret_cast<void*>(inputWasKeyReleased));
    mono_add_internal_call("Aderite.Input::__IsMouseButtonDown(Aderite.MouseKey)", reinterpret_cast<void*>(inputIsMouseButtonDown));
    mono_add_internal_call("Aderite.Input::__GetMousePosition()", reinterpret_cast<void*>(inputGetMousePosition));
    mono_add_internal_call("Aderite.Input::__GetMouseDelta()", reinterpret_cast<void*>(inputGetMouseDelta));
    mono_add_internal_call("Aderite.Input::__GetScrollDelta()", reinterpret_cast<void*>(inputGetScrollDelta));
}
} // namespace input

} // namespace internal_

void linkInput() {
    // Input
    internal_::input::linkInput();
}
