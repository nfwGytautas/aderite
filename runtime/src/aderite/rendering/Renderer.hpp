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
#include "aderite/scene/components/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/rendering/pass/Forward.hpp"
#include "aderite/rendering/uniform/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief The Renderer of aderite powered by bgfx
*/
class Renderer {
	using DrawCallList = std::vector<DrawCall>;

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

	void renderFrom(scene::Scene* scene, interfaces::ICamera* eye);

	/**
	 * @brief Validates entity
	 * @param meta Meta component
	 * @param mrenderer Mesh renderer component
	 * @return True if valid and can be rendered, false otherwise
	*/
	bool validateEntity(
		scene::components::MetaComponent& meta, 
		scene::components::MeshRendererComponent& mrenderer);

	/**
	 * @brief Frustum cull entity
	 * @param eye Camera
	 * @param transform Transform of the entity
	 * @return True if not culled, false otherwise
	*/
	bool frustumCull(
		interfaces::ICamera* eye,
		scene::components::TransformComponent& transform);

	/**
	 * @brief Performs occlusion culling for the draw calls
	 * @param dcl Draw call list
	*/
	void occlusionCull(
		DrawCallList& dcl
	);

	/**
	 * @brief Performs optimizations for the draw call lit
	 * @param dc Draw call
	*/
	void optimize(
		DrawCall& dcl
	);

	/**
	 * @brief Renders the draw call
	 * @param dc Draw call to render
	*/
	void render(const DrawCall& dc);
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
