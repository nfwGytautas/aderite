#include "EditorCamera.hpp"

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/Aderite.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/shared/Settings.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor {

EditorCamera::EditorCamera() {
    const uint64_t flags = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP |
                           BGFX_SAMPLER_V_CLAMP | BGFX_TEXTURE_BLIT_DST;

    m_output = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, false, 1, bgfx::TextureFormat::BGRA8, flags);
}

EditorCamera::~EditorCamera() {
    bgfx::destroy(m_output);
}

bgfx::TextureHandle EditorCamera::getOutputHandle() const {
    return m_output;
}

void EditorCamera::onViewportResize(const glm::uvec2& size) {
    if (size.x == 0 || size.y == 0) {
        return;
    }

    m_viewportSize = size;
}

void EditorCamera::update(float delta) {
    auto inputManager = ::aderite::Engine::getInputManager();

    if (inputManager->isKeyPressed(input::Key::LEFT_ALT)) {
        // Zoom
        double scrollDelta = inputManager->getScrollDelta();

        if (scrollDelta != 0.0) {
            m_distance -= delta * scrollDelta * zoomSpeed();
            if (m_distance < 1.0f) {
                m_focalpoint += getForwardDirection();
                m_distance = 1.0f;
            }
        }

        // Pan
        if (inputManager->isMouseKeyPressed(input::MouseKey::MIDDLE)) {
            glm::vec2 mouseDelta = inputManager->getMouseDelta();

            glm::vec2 speed = panSpeed();
            m_focalpoint += -getRightDirection() * mouseDelta.x * speed.x * m_distance * delta;
            m_focalpoint += getUpDirection() * mouseDelta.y * speed.y * m_distance * delta;
        }
    } else {
        // Rotate
        if (inputManager->isMouseKeyPressed(input::MouseKey::MIDDLE)) {
            glm::vec2 mouseDelta = inputManager->getMouseDelta();

            float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
            m_currentEulerRotation.x += delta * mouseDelta.y * Settings::EditorCameraRotationSpeed;
            m_currentEulerRotation.y -= delta * yawSign * mouseDelta.x * Settings::EditorCameraRotationSpeed;
        }
    }

    // Add to frame data
    rendering::FrameData& fd = ::aderite::Engine::getRenderer()->getWriteFrameData();

    // Fill camera data
    rendering::CameraData cd;
    cd.Name = "Editor";
    cd.Output = m_output;
    cd.ProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
    cd.ViewMatrix =
        glm::inverse(glm::translate(glm::mat4(1.0f), this->calculatePosition()) * glm::toMat4(glm::quat(m_currentEulerRotation)));

    // Push to the list
    fd.Cameras.push_back(cd);
}

glm::vec3 EditorCamera::getUpDirection() const {
    return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 EditorCamera::getRightDirection() const {
    return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 EditorCamera::getForwardDirection() const {
    return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 EditorCamera::calculatePosition() const {
    return m_focalpoint - getForwardDirection() * m_distance;
}

glm::quat EditorCamera::getOrientation() const {
    return glm::quat(m_currentEulerRotation);
}

float EditorCamera::zoomSpeed() const {
    float distance = m_distance * Settings::EditorCameraZoomSpeed;
    distance = std::max(distance, 0.0f);
    float speed = distance * distance;
    speed = std::min(speed, 100.0f); // max speed = 100
    return speed;
}

glm::vec2 EditorCamera::panSpeed() const {
    float x = std::min(m_viewportSize.x / 1000.0f, 2.4f); // max = 2.4f
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min(m_viewportSize.y / 1000.0f, 2.4f); // max = 2.4f
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return {xFactor, yFactor};
}

} // namespace editor
} // namespace aderite
