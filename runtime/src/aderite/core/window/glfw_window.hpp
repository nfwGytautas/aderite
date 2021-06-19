#pragma once

#include "GLFW/glfw3.h"
#include "aderite/core/window/window.hpp"
#include "aderite/core/window/window_manager.hpp"

namespace aderite {
	namespace window_backend {
		namespace glfw {

			/**
			 * @brief GLFW implementation of the generic window
			*/
			class glfw_window : public window {
			public:
				void destroy() override;
				void make_active() override;
				void begin_frame() override;
				void end_frame() override;

				GLFWwindow* get_handle() {
					return m_window;
				}

			private:
				glfw_window(create_options options);
				friend class window_manager;
			private:
				GLFWwindow* m_window;
			};

			/**
			 * @brief Initializes glfw windowing backend
			*/
			bool init_backend(window_manager* ws);

			/**
			 * @brief Shutdown glfw windowing backend
			*/
			void shutdown_backend(window_manager* ws);

			/**
			 * @brief Poll native input system
			*/
			void poll_events(window_manager* ws);
		}
	}
}