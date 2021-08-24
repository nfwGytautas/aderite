#include "WindowManager.hpp"

#include "aderite/config.hpp"
#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/rendering/Renderer.hpp"

#if GLFW_BACKEND
#include "aderite/window/glfw_window.hpp"

#define f_init init_backend
#define f_shutdown shutdown_backend
#define f_create glfw_window
#define f_poll poll_events

#endif

ADERITE_WINDOW_NAMESPACE_BEGIN

static WindowManager* manager_instance = nullptr;

bool WindowManager::init() {
	LOG_TRACE("Initializing window manager");
	manager_instance = this;

	if (!f_init(this)) {
		LOG_ERROR("Failed to initialize backend");
		return false;
	}

	return true;
}

void WindowManager::shutdown() {
	for (window* wnd : m_windows) {
		wnd->destroy();
		delete wnd;
	}

	m_windows.clear();

	f_shutdown(this);
}

window* WindowManager::create_window(window::create_options options) {
	window* wnd = nullptr;

	// Initialize rest of the systems on rendering thread
	wnd = f_create(options);
	wnd->make_active();

	// Run Renderer init on this window
	engine::get()->get_renderer()->init(wnd);

	m_windows.push_back(wnd);

	return wnd;
}

void WindowManager::active_window_changed(window* window) {
	m_current_window = window;
}

void WindowManager::begin_frame() {
	for (auto& window : m_windows) {
		window->begin_frame();
	}
}

void WindowManager::end_frame() {
	for (auto& window : m_windows) {
		window->end_frame();
	}

	f_poll(this);
}

ADERITE_WINDOW_NAMESPACE_END
