#include "window_manager.hpp"

#include "aderite/config.hpp"
#include "aderite/core/aderite.hpp"
#include "aderite/utility/log.hpp"

#if GLFW_BACKEND
#include "aderite/core/window/glfw_window.hpp"

#define f_init window_backend::glfw::init_backend
#define f_shutdown window_backend::glfw::shutdown_backend
#define f_create new window_backend::glfw::glfw_window
#define f_poll window_backend::glfw::poll_events

#endif

namespace aderite {

	static window_manager* manager_instance = nullptr;

	bool window_manager::init() {
		LOG_TRACE("Initializing window manager");
		manager_instance = this;

		if (!f_init(this)) {
			LOG_ERROR("Failed to initialize backend");
			return false;
		}

		return true;
	}

	void window_manager::shutdown() {
		for (const ref<window>& wnd : m_windows) {
			wnd->destroy();
		}

		f_shutdown(this);
	}

	ref<window> window_manager::create_window(window::create_options options) {
		ref<window> wnd = nullptr;

		// Initialize rest of the systems on rendering thread
		ref<thread::void_invoke> invoke = new thread::void_invoke([&]() {
			wnd = f_create(options);
			wnd->make_active();

			// Run renderer init on this window
			engine::get()->get_renderer()->init();

			m_windows.push_back(wnd);
		});

		engine::get()->get_threader()->get_render_thread()->invoke(invoke.as<thread::thread_invoke_base>());
		invoke->wait();

		return wnd;
	}

	void window_manager::active_window_changed(relay_ptr<window> window) {
		m_current_window = window;
	}

	void window_manager::begin_frame() {
		for (auto& window : m_windows) {
			window->begin_frame();
		}
	}

	void window_manager::end_frame() {
		for (auto& window : m_windows) {
			window->end_frame();
		}

		f_poll(this);
	}
}
