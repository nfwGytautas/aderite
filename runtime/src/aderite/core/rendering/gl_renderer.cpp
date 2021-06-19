#include "gl_renderer.hpp"

#include "aderite/config.hpp"
#include "aderite/utility/log.hpp"

#if GLFW_BACKEND
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

namespace aderite {
	namespace render_backend {
		namespace opengl {

			bool gl_renderer::init() {
				LOG_DEBUG("OpenGL renderer");
				
#if GLFW_BACKEND
				if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
					LOG_ERROR("Failed to initialize GLAD, make sure a valid rendering context is valid");
					return false;
				}
#endif

				return true;
			}

			void gl_renderer::shutdown() {

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

			}

			void gl_renderer::end_frame() {

			}

		}
	}
}