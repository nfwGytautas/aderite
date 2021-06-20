#include "glfw_window.hpp"

#include "GLFW/glfw3.h"
#include "aderite/utility/log.hpp"

namespace aderite {
	namespace window_backend {
		namespace glfw {

			glfw_window::glfw_window(create_options options)
				: m_window(nullptr)
			{
				m_window = glfwCreateWindow(options.width, options.height, options.title, NULL, NULL);


			}

			void glfw_window::destroy() {
				glfwDestroyWindow(m_window);
			}

			void glfw_window::make_active() {
				glfwMakeContextCurrent(m_window);
				window::make_active();
			}

			void glfw_window::begin_frame() {

			}

			void glfw_window::end_frame() {
				closed = glfwWindowShouldClose(m_window);

				// Swap buffer
				glfwSwapBuffers(m_window);
			}

			bool init_backend(window_manager* ws) {
				LOG_DEBUG("GLFW backend");

				glfwSetErrorCallback([](int error, const char* description) {
					LOG_ERROR("GLFW error: {0}, {1}", error, description);
				});

				if (!glfwInit()) {
					return false;
				}

#ifndef __DEBUG
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);

				// Debug mode for OpenGL
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#else
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#endif

				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

				return true;
			}

			void shutdown_backend(window_manager* ws) {
				glfwTerminate();
			}

			void poll_events(window_manager* ws) {
				glfwPollEvents();
			}

		}
	}
}
