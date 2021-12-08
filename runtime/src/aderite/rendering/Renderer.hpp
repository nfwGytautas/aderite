#pragma once

#include <string>

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
class Engine;

namespace rendering {

/**
 * @brief The Renderer of aderite powered by bgfx
 */
class Renderer final {
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
     * @brief Executes the rendering pipeline
     */
    void render();

    /**
     * @brief Returns true if Renderer ready to render
     */
    bool isReady() const;

    /**
     * @brief Sets the resolution of the renderer
     * @param size The new resolution
     */
    void setResolution(const glm::uvec2& size);

    /**
     * @brief Advances to next frame
     */
    void commit() const;

    /**
     * @brief Sets the editor camera instance
     * @param camera Camera of the editor
     */
    void setEditorCamera(scene::Camera* camera);

private:
    /**
     * @brief Creates render targets of the renderer
     */
    bool createTargets();

    /**
     * @brief Prepares targets for a render pass (clears information)
     */
    void prepareTargets();

    /**
     * @brief Sets up the view index
     * @param idx View index
     * @param name Name of the view
     */
    void setupView(uint8_t idx, const std::string& name);

private:
    Renderer() {}
    friend Engine;

private:
    bool m_isInitialized = false;

    scene::Camera* m_editorCamera = nullptr;

    // BGFX views
    glm::uvec2 m_resolution = glm::uvec2(1280, 920);

    // Targets

    /**
     * @brief This will be the target where the result will be stored
     */
    bgfx::FrameBufferHandle m_mainFbo = BGFX_INVALID_HANDLE;
};

} // namespace rendering
} // namespace aderite
