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

		// Asset manager
		m_asset_manager = new asset::asset_manager();
		if (!m_asset_manager->init()) {
			LOG_ERROR("Aborting aderite initialization");
			return false;
		}

		// Window system
		m_window_manager = new window_manager();
		if (!m_window_manager->init()) {
			LOG_ERROR("Aborting aderite initialization");
			return false;
		}

		// Renderer
		m_renderer = renderer::create_instance(); // Delay init for until there are windows

		return true;
	}

	void engine::shutdown() {
		m_asset_manager->shutdown();
		m_renderer->shutdown();
		m_window_manager->shutdown();

		delete m_asset_manager;
		delete m_renderer;
		delete m_window_manager;
	}

	void engine::loop() {
		while (!m_wants_to_shutdown) {
			begin_frame();
			m_renderer->render();
			end_frame();
		}
	}

	void engine::request_exit() {
		m_wants_to_shutdown = true;
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