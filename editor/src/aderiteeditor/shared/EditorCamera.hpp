#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor {

class EditorCamera {
public:
    EditorCamera();
    virtual ~EditorCamera();

    /**
     * @brief Called when scene view is resized
     * @param size New size
     */
    void onViewportResize(const glm::uvec2& size);

    /**
     * @brief Returns the view matrix of the editor camera
     */
    const glm::mat4& getViewMatrix() const;

    /**
     * @brief Returns the projection matrix of the editor camera
     */
    const glm::mat4& getProjectionMatrix() const;

    /**
     * @brief Updates the editor camera
     * @param delta Delta of the last frame
     */
    void update(float delta);

    /**
     * @brief Returns true if the editor camera is currently enabled, false otherwise
     */
    bool isEnabled() const;

private:
    glm::vec3 getUpDirection() const;
    glm::vec3 getRightDirection() const;
    glm::vec3 getForwardDirection() const;
    glm::vec3 calculatePosition() const;
    glm::quat getOrientation() const;

    void updateViewMatrix();

    float zoomSpeed() const;
    glm::vec2 panSpeed() const;

private:
    glm::uvec2 m_viewportSize = {};
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;

    glm::vec3 m_focalpoint = {};
    float m_pitch = 0.0f;
    float m_yaw = 0.0f;
    float m_distance = 10.0f;
};

} // namespace editor
} // namespace aderite
