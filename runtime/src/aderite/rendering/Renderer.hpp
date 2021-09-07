#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include "aderite/config.hpp"
#include "aderite/scene/ICamera.hpp"
#include "aderite/rendering/pass/IPass.hpp"
#include "aderite/window/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/rendering/pass/Forward.hpp"
#include "aderite/rendering/uniform/Forward.hpp"

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
	 * @brief Function invoked when the window has been resized
	 * @param newWidth New width of the window
	 * @param newHeight New height of the window
	*/
	void onWindowResized(unsigned int newWidth, unsigned int newHeight, bool reset = true);

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

	/**
	 * @brief Advances to next frame
	*/
	void commit();

	/**
	 * @brief Returns the uniform manager for the renderer
	*/
	uniform::UniformManager* getUniformManager() const;

#if DEBUG_RENDER == 1
	/**
	 * @brief Returns the debug pass instance
	*/
	pass::DebugPass* getDebugPass() {
		return m_debugPass;
	}
#endif
private:
	Renderer() {}
	friend class Engine;

	void renderFrom(interfaces::ICamera* eye);
private:
	static constexpr uint8_t c_NumPasses = 2;
	std::array<interfaces::IPass*, c_NumPasses> m_passes;

	uniform::UniformManager* m_uManager = nullptr;

	bool m_isInitialized = false;

#if DEBUG_RENDER == 1
	pass::DebugPass* m_debugPass = nullptr;
#endif
};

ADERITE_RENDERING_NAMESPACE_END
