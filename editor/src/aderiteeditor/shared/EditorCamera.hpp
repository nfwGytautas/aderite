#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace editor {

class EditorCamera {
public:
    EditorCamera();
    ~EditorCamera();

    /**
     * @brief Returns the camera instance that is being manipulated
     */
    scene::Camera* getCamera() const;

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
    scene::Camera* m_camera = nullptr;
    glm::uvec2 m_viewportSize = {};

    glm::vec3 m_focalpoint = {};
    float m_distance = 10.0f;
};

} // namespace editor
} // namespace aderite
