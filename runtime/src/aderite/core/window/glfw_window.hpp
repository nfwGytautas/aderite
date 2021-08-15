#pragma once

#include "aderite/core/window/window.hpp"

class GLFWwindow;

namespace aderite {
	class window_manager;

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
				virtual glm::vec2 get_size() override;
				virtual void set_title(const std::string& title) override;

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