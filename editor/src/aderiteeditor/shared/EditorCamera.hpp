#pragma once

#include <bgfx/bgfx.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace aderite {
namespace editor {

class EditorCamera {
public:
    EditorCamera();
    ~EditorCamera();

    /**
     * @brief Returns the output handle of the editor camera
     */
    bgfx::TextureHandle getOutputHandle() const;

    /**
     * @brief Called when scene view is resized
     * @param size New size
     */
    void onViewportResize(const glm::uvec2& size);

    /**
     * @brief Updates the editor camera
     * @param delta Delta of the last frame
     */
    void update(float delta);

private:
    glm::vec3 getUpDirection() const;
    glm::vec3 getRightDirection() const;
    glm::vec3 getForwardDirection() const;
    glm::vec3 calculatePosition() const;
    glm::quat getOrientation() const;

    float zoomSpeed() const;
    glm::vec2 panSpeed() const;

private:
    bgfx::TextureHandle m_output = BGFX_INVALID_HANDLE;
    glm::uvec2 m_viewportSize = {};

    glm::vec3 m_focalpoint = {};
    glm::vec3 m_currentEulerRotation = {};
    float m_distance = 10.0f;
};

} // namespace editor
} // namespace aderite
