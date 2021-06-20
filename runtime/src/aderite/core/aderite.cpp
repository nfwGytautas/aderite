#include "aderite.hpp"

#include "aderite/utility/log.hpp"
#include "aderite/core/threading/thread_invoke.hpp"

constexpr char* version = "0.0.0";

namespace aderite {

	engine* engine::get() {
		static engine* instance = new engine();
		return instance;
	}

	bool engine::init(init_options options) {
		// First init logger
		logger::get()->init();

		LOG_TRACE("Initializing aderite engine");
		LOG_DEBUG("Version: {0}", version);

		// Threader
		m_threader = new thread::threader();
		if (!m_threader->init()) {
			LOG_ERROR("Aborting aderite initialization");
			return false;
		}

		// Initialize rest of the systems on rendering thread
		ref<thread::f_invoke<bool>> invoke = new thread::f_invoke<bool>([&]() {
			// Window system
			m_window_manager = new window_manager();
			if (!m_window_manager->init()) {
				return false;
			}

			// Renderer
			m_renderer = renderer::create_instance();
			// Delay init for until there are windows

			return true;
		});

		m_threader->get_render_thread()->invoke(invoke.as<thread::thread_invoke_base>());
		invoke->wait();
		bool result = *(bool*)invoke->result();
		if (!result) {
			LOG_ERROR("Aborting aderite initialization");
			return false;
		}

		return true;
	}

	void engine::shutdown() {
		m_window_manager->shutdown();
		m_renderer->shutdown();
	}

	void engine::loop() {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_exit_cond.wait(lock, [this]() { return m_wants_to_shutdown; });
		m_threader->get_render_thread()->join();
	}

	void engine::request_exit() {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_wants_to_shutdown = true;
		m_exit_cond.notify_all();
	}

	void engine::begin_frame() {
		m_window_manager->begin_frame();
		m_renderer->begin_frame();
	}

	void engine::end_frame() {
		m_renderer->end_frame();
		m_window_manager->end_frame();
	}

}