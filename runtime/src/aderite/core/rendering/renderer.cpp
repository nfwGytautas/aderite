#include "renderer.hpp"

#include "aderite/config.hpp"
#include "aderite/utility/log.hpp"

#if GL_BACKEND
#include "aderite/core/rendering/gl_renderer.hpp"
#endif

namespace aderite {

	renderer* renderer::create_instance() {
		LOG_TRACE("Creating renderer instance");
#if GL_BACKEND
		return new render_backend::opengl::gl_renderer();
#endif

		LOG_ERROR("No rendering backend specified");
		return nullptr;
	}

	bool renderer::init(relay_ptr<window> wnd) {
		// Setup default render target
		m_attached_to = wnd;
		return true;
	}

	void renderer::shutdown() {
		for (auto& layer : m_layers) {
			layer->i_shutdown();
		}

		m_layers.clear();
	}

	void renderer::render() {
		if (!ready()) {
			return;
		}

		for (auto& layer : m_layers) {
			// TODO: Layer ordering
			if (layer->ready()) {
				// Bind target
				set_output(layer->target);
				layer->render();
			}
		}

		// Restore default
		set_output(nullptr);
	}

	void renderer::set_output(relay_ptr<fbo> target) {
		if (!ready()) {
			return;
		}

		if (target.valid()) {
			target->bind();
		}
		else {
			output_to_default();
		}
	}

	void renderer::set_default_target(relay_ptr<fbo> target) {
		m_default_target = target;
	}

}