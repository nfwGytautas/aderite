#pragma once

#include <vector>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include "aderite/window/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/rendering/Forward.hpp"

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
	 * @brief Submit a scene to be rendered
	 * @param scene Scene to render
	*/
	void renderScene(scene::Scene* scene);

	/**
	 * @brief Returns true if Renderer ready to render
	*/
	bool isReady();

	/**
	 * @brief Sets the resolution of the renderer
	 * @param size The new resolution
	*/
	void setResolution(const glm::uvec2& size);

	/**
	 * @brief Renders output of a framebuffer to screen
	 * @param image Image to display
	*/
	void displayFrame(bgfx::FrameBufferHandle image);
private:
	Renderer() {}
	friend class Engine;

	void executeDrawCall(DrawCall& dc);
private:
	bool m_isInitialized = false;
	size_t m_lastRenderDrawCalls = 0;
};

ADERITE_RENDERING_NAMESPACE_END
