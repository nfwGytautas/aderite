#pragma once

#include <vector>
#include "aderite/utility/Macros.hpp"
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
	void beginFrame();
	void endFrame();

	/**
	 * @brief Creates a new window with specified options
	 * @param options Options to create with
	*/
	Window* createWindow(Window::CreateOptions options);

	/**
	 * @brief Function gets called when the current active window is changed by calling window->makeActive
	*/
	void onActiveWindowChanged(Window* window);

	/**
	 * @brief Returns the current active window
	*/
	Window* getCurrentActiveWindow() {
		return m_currentWindow;
	}
private:
	WindowManager() {}
	friend class Engine;

private:
	std::vector<Window*> m_windows;
	Window* m_currentWindow = nullptr;
};

ADERITE_WINDOW_NAMESPACE_END
