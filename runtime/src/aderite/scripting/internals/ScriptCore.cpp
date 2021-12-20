#include "ScriptCore.hpp"

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scripting/ScriptClass.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace aderite {
namespace scripting {

bool inputIsKeyDown(input::Key key) {
    return ::aderite::Engine::getInputManager()->isKeyPressed(key);
}

bool inputWasKeyReleased(input::Key key) {
    return ::aderite::Engine::getInputManager()->wasKeyReleased(key);
}

bool inputIsMouseButtonDown(input::MouseKey key) {
    return ::aderite::Engine::getInputManager()->isMouseKeyPressed(key);
}

glm::vec2 inputGetMousePosition() {
    return ::aderite::Engine::getInputManager()->getMousePosition();
}

glm::vec2 inputGetMouseDelta() {
    return ::aderite::Engine::getInputManager()->getMouseDelta();
}

double inputGetScrollDelta() {
    return ::aderite::Engine::getInputManager()->getScrollDelta();
}

MonoObject* engineGetScript(MonoString* type) {
    ScriptClass* script = ::aderite::Engine::getScriptManager()->getScript(mono_string_to_utf8(type));

    if (script != nullptr) {
        return script->getInstance();
    }

    return nullptr;
}

void coreInternals() {
    // Input
    mono_add_internal_call("Aderite.Input::__IsKeyDown(Aderite.Key)", reinterpret_cast<void*>(inputIsKeyDown));
    mono_add_internal_call("Aderite.Input::__WasKeyReleased(Aderite.Key)", reinterpret_cast<void*>(inputWasKeyReleased));
    mono_add_internal_call("Aderite.Input::__IsMouseButtonDown(Aderite.MouseKey)", reinterpret_cast<void*>(inputIsMouseButtonDown));
    mono_add_internal_call("Aderite.Input::__GetMousePosition()", reinterpret_cast<void*>(inputGetMousePosition));
    mono_add_internal_call("Aderite.Input::__GetMouseDelta()", reinterpret_cast<void*>(inputGetMouseDelta));
    mono_add_internal_call("Aderite.Input::__GetScrollDelta()", reinterpret_cast<void*>(inputGetScrollDelta));

    // Engine
    mono_add_internal_call("Aderite.Engine::__GetScript(string)", reinterpret_cast<void*>(engineGetScript));
}

} // namespace scripting
} // namespace aderite
