#include "gl_renderer.hpp"

#include "aderite/config.hpp"

#if GLFW_BACKEND
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/core/rendering/fbo/fbo.hpp"
#include "aderite/core/window/window.hpp"

namespace aderite {
	namespace render_backend {
		namespace opengl {

			bool gl_renderer::init(window* wnd) {
				if (!renderer::init(wnd)) {
					return false;
				}

				LOG_DEBUG("OpenGL renderer");
				
#if GLFW_BACKEND
				if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
					LOG_ERROR("Failed to initialize GLAD, make sure a valid rendering context is valid");
					return false;
				}
#endif

				m_initialized = true;
				engine::get()->renderer_initialized();
				return true;
			}

			void gl_renderer::shutdown() {
				renderer::shutdown();
			}

			void gl_renderer::set_vsync(bool enabled) {
#if GLFW_BACKEND
				if (enabled) {
					glfwSwapInterval(1);
				}
				else {
					glfwSwapInterval(0);
				}
#endif
			}

			void gl_renderer::begin_frame() {
				if (!ready()) {
					return;
				}

			}

			void gl_renderer::end_frame() {
				if (!ready()) {
					return;
				}

			}

			bool gl_renderer::ready() {
				return m_initialized;
			}

			void gl_renderer::clear() {
				constexpr float v = 37.0f / 255.0f;
				glClearColor(v, v, v, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

			void gl_renderer::output_to_default() {
				if (m_default_target != nullptr) {
					reset_output();
					return;
				}

				m_default_target->bind();
			}

			void gl_renderer::reset_output() {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// Set viewport
				glm::vec2 size = m_attached_to->get_size();
				glViewport(0, 0, (int)size.x, (int)size.y);
			}

		}
	}
}