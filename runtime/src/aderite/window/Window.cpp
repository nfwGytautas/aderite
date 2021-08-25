#include "Window.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/window/WindowManager.hpp"

ADERITE_WINDOW_NAMESPACE_BEGIN

void Window::makeActive() {
	::aderite::Engine::get()->getWindowManager()->onActiveWindowChanged(this);
}

ADERITE_WINDOW_NAMESPACE_END
