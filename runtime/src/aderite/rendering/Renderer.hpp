#pragma once

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include "aderite/config.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/window/Forward.hpp"

namespace aderite {
class Engine;

namespace rendering {
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
     * @brief Executes the rendering pipeline
     */
    void render();

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
     * @brief Advances to next frame
     */
    void commit();

    /**
     * @brief Returns the rendering pipeline instance
     * @return Pipeline instance
     */
    Pipeline* getPipeline() const;

    /**
     * @brief Set the pipeline for the renderer to use
     * @param pipeline Pipeline to use
     */
    void setPipeline(Pipeline* pipeline);

private:
    Renderer() {}
    friend Engine;

private:
    Pipeline* m_pipeline = nullptr;
    bool m_isInitialized = false;
};

} // namespace rendering
} // namespace aderite
