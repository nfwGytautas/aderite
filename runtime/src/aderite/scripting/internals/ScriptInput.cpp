#include "ScriptInput.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/tokentype.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/input/InputManager.hpp"

namespace aderite {
namespace scripting {

bool IsKeyDown(input::Key key) {
	return ::aderite::Engine::getInputManager()->isKeyPressed(key);
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
	mono_add_internal_call("Aderite.Input::__IsKeyDown(Aderite.Key)", IsKeyDown);
	mono_add_internal_call("Aderite.Input::__IsMouseButtonDown(Aderite.MouseKey)", IsMouseButtonDown);
	mono_add_internal_call("Aderite.Input::__GetMousePosition()", GetMousePosition);
	mono_add_internal_call("Aderite.Input::__GetMouseDelta()", GetMouseDelta);
	mono_add_internal_call("Aderite.Input::__GetScrollDelta()", GetScrollDelta);
}

}
}