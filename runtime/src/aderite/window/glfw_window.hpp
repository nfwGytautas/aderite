#pragma once

#include <string>
#include "aderite/window/Window.hpp"

class GLFWwindow;

ADERITE_WINDOW_NAMESPACE_BEGIN

// TODO: Rethink this

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
	virtual void* get_native_handle() override;

	GLFWwindow* get_glfw_window() {
		return m_window;
	}
private:
	glfw_window(create_options options);
	friend class WindowManager;
private:
	GLFWwindow* m_window;
};

/**
 * @brief Initializes glfw windowing backend
*/
bool init_backend(WindowManager* ws);

/**
 * @brief Shutdown glfw windowing backend
*/
void shutdown_backend(WindowManager* ws);

/**
 * @brief Poll native input system
*/
void poll_events(WindowManager* ws);

ADERITE_WINDOW_NAMESPACE_END
