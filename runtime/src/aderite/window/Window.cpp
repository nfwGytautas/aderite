#include "Window.hpp"

#include "aderite/aderite.hpp"
#include "aderite/window/WindowManager.hpp"

ADERITE_WINDOW_NAMESPACE_BEGIN

void window::make_active() {
	engine::get()->get_window_manager()->active_window_changed(this);
}

ADERITE_WINDOW_NAMESPACE_END
