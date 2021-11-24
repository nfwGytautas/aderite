#include "ScriptInput.hpp"

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/input/InputManager.hpp"

namespace aderite {
namespace scripting {

bool IsKeyDown(input::Key key) {
    return ::aderite::Engine::getInputManager()->isKeyPressed(key);
}

bool WasKeyReleased(input::Key key) {
    return ::aderite::Engine::getInputManager()->wasKeyReleased(key);
}

bool IsMouseButtonDown(input::MouseKey key) {
    return ::aderite::Engine::getInputManager()->isMouseKeyPressed(key);
}

glm::vec2 GetMousePosition() {
    return ::aderite::Engine::getInputManager()->getMousePosition();
}

glm::vec2 GetMouseDelta() {
    return ::aderite::Engine::getInputManager()->getMouseDelta();
}

double GetScrollDelta() {
    return ::aderite::Engine::getInputManager()->getScrollDelta();
}

void inputInternals() {
    mono_add_internal_call("Aderite.Input::__IsKeyDown(Aderite.Key)", reinterpret_cast<void*>(IsKeyDown));
    mono_add_internal_call("Aderite.Input::__WasKeyReleased(Aderite.Key)", reinterpret_cast<void*>(WasKeyReleased));
    mono_add_internal_call("Aderite.Input::__IsMouseButtonDown(Aderite.MouseKey)", reinterpret_cast<void*>(IsMouseButtonDown));
    mono_add_internal_call("Aderite.Input::__GetMousePosition()", reinterpret_cast<void*>(GetMousePosition));
    mono_add_internal_call("Aderite.Input::__GetMouseDelta()", reinterpret_cast<void*>(GetMouseDelta));
    mono_add_internal_call("Aderite.Input::__GetScrollDelta()", reinterpret_cast<void*>(GetScrollDelta));
}

} // namespace scripting
} // namespace aderite
