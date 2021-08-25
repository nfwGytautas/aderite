#include "WindowManager.hpp"

#include "aderite/Config.hpp"
#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/rendering/Renderer.hpp"

#if GLFW_BACKEND
#include "aderite/window/glfw_window.hpp"

#define f_init init_backend
#define f_shutdown shutdown_backend
#define f_create new glfw_window
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
	for (Window* wnd : m_windows) {
		wnd->destroy();
		delete wnd;
	}

	m_windows.clear();

	f_shutdown(this);
}

Window* WindowManager::createWindow(Window::CreateOptions options) {
	Window* wnd = nullptr;

	// Initialize rest of the systems on rendering thread
	wnd = f_create(options);
	wnd->makeActive();

	// Run Renderer init on this window
	::aderite::Engine::get()->getRenderer()->init(wnd);

	m_windows.push_back(wnd);

	return wnd;
}

void WindowManager::onActiveWindowChanged(Window* window) {
	m_currentWindow = window;
}

void WindowManager::beginFrame() {
	for (auto& window : m_windows) {
		window->beginFrame();
	}
}

void WindowManager::endFrame() {
	for (auto& window : m_windows) {
		window->endFrame();
	}

	f_poll(this);
}

ADERITE_WINDOW_NAMESPACE_END
