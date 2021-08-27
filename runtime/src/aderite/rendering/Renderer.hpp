#pragma once

#include <vector>
#include <bgfx/bgfx.h>
#include "aderite/window/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief The Renderer of aderite powered by bgfx
*/
class Renderer {
public:
	virtual ~Renderer() {}

	/**
	 * @brief Initializes the Renderer
	*/
	bool init();

	/**
	 * @brief Shutdown Renderer
	*/
	void shutdown();

	/**
	 * @brief Enables or disables vsync
	*/
	void setVsync(bool enabled);

	/**
	 * @brief Clears the current output target
	*/
	void clear();

	/**
	 * @brief Resets the Renderer output to be the window
	*/
	void resetOutput();

	/**
	 * @brief Executes a single render pass
	*/
	void render();

	/**
	 * @brief Returns true if Renderer ready to render
	*/
	bool isReady();

	// TODO: Rethink this
	void beginFrame();
	void endFrame();

	/**
	 * @brief Returns the final output framebuffer which can then be rendered to texture in editors, call displayFrame()
	 * to render the content of the framebuffer to screen
	*/
	bgfx::FrameBufferHandle getOutput();

	/**
	 * @brief Renders output framebuffer to screen
	*/
	void displayFrame();
private:
	Renderer() {}
	friend class Engine;

private:
	bool m_isInitialized = false;
	bgfx::FrameBufferHandle m_output = BGFX_INVALID_HANDLE;
};

ADERITE_RENDERING_NAMESPACE_END
