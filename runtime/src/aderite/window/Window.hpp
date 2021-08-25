#pragma once

#include <functional>
#include <glm/glm.hpp>
#include "aderite/utility/Macros.hpp"

ADERITE_WINDOW_NAMESPACE_BEGIN

/**
 * @brief Generic window class
*/
class Window {
public:
	/**
	 * @brief Window create options passed to createWindow, most can be changed at runtime
	*/
	struct CreateOptions {
		int width = 1280;
		int height = 720;
		const char* title = "Window";
	};

public:
	bool closed = false;

public:
	Window() {}
	virtual ~Window() {}

	/**
	 * @brief Destroys the window
	*/
	virtual void destroy() = 0;

	/**
	 * @brief Makes this window the current active one where rendering will be submitted to
	*/
	virtual void makeActive();

	/**
	 * @brief Set the title of the window
	*/
	virtual void setTitle(const std::string& title) = 0;

	/**
	 * @brief Returns the native platform window handle
	 * @return Type depends on the platform for example on windows this will be a HWND
	*/
	virtual void* getNativeHandle() = 0;

	/**
	 * @brief Returns the size of the window
	 * @return GLM vector 2D (width, height)
	*/
	virtual glm::vec2 getSize() = 0;

	// TODO: Rethink this
	virtual void beginFrame() {}
	virtual void endFrame() {}
private:
	Window(const Window& o) = delete;
	friend class WindowManager;
};

ADERITE_WINDOW_NAMESPACE_END
