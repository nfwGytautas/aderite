#include "aderite.hpp"

#include "aderite/utility/log.hpp"

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

		// Window system
		m_window_manager = new window_manager();
		if (!m_window_manager->init()) {
			LOG_ERROR("Aborting aderite initialization");
			return false;
		}

		// Renderer
		m_renderer = renderer::create_instance();
		// Delay init for until there are windows

		return true;
	}

	void engine::shutdown() {
		m_window_manager->shutdown();
		m_renderer->shutdown();
	}

	void engine::begin_frame() { 

	}

	void engine::end_frame() {
		m_renderer->end_frame();
		m_window_manager->end_frame();
	}

}