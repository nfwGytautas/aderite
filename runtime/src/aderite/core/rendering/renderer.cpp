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

	void renderer::shutdown() {
		for (auto& layer : m_layers) {
			layer->i_shutdown();
		}
	}

	void renderer::render() {
		for (auto& layer : m_layers) {
			// TODO: Layer ordering
			if (layer->ready()) {
				layer->render();
			}
		}
	}

}