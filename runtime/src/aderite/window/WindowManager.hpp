#pragma once

#include <vector>
#include "aderite/utility/macros.hpp"
#include "aderite/window/Window.hpp"

ADERITE_WINDOW_NAMESPACE_BEGIN

/**
 * @brief Window manager of aderite engine, usually most applications will only use 1 window
 * and as such the window manager is built with that
*/
class WindowManager {
public:
	/**
	 * @brief Initializes the window manager
	*/
	bool init();

	/**
	 * @brief Shutdown the window manager
	*/
	void shutdown();

	// TODO: Rethink this
	void begin_frame();
	void end_frame();

	/**
	 * @brief Creates a new window with specified options
	 * @param options Options to create with
	*/
	window* create_window(window::create_options options);

	/**
	 * @brief Function gets called when the current active window is changed by calling window->make_active
	*/
	void active_window_changed(window* window);

	/**
	 * @brief Returns the current active window
	*/
	window* get_current_active_window() {
		return m_current_window;
	}
private:
	WindowManager() {}
	friend class engine;

private:
	std::vector<window*> m_windows;
	window* m_current_window = nullptr;
};

ADERITE_WINDOW_NAMESPACE_END
