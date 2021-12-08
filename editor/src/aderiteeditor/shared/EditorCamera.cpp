#include "EditorCamera.hpp"

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/Aderite.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/shared/Settings.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor {

EditorCamera::EditorCamera() {
    m_camera = new scene::Camera();
    m_camera->setName("Editor");
}

EditorCamera::~EditorCamera() {
    delete m_camera;
}

scene::Camera* EditorCamera::getCamera() const {
    return m_camera;
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
            m_camera->setPosition(this->calculatePosition());
        }

        // Pan
        if (inputManager->isMouseKeyPressed(input::MouseKey::MIDDLE)) {
            glm::vec2 mouseDelta = inputManager->getMouseDelta();

            glm::vec2 speed = panSpeed();
            m_focalpoint += -getRightDirection() * mouseDelta.x * speed.x * m_distance * delta;
            m_focalpoint += getUpDirection() * mouseDelta.y * speed.y * m_distance * delta;
            m_camera->setPosition(this->calculatePosition());
        }
    } else {
        // Rotate
        if (inputManager->isMouseKeyPressed(input::MouseKey::MIDDLE)) {
            glm::vec2 mouseDelta = inputManager->getMouseDelta();

            float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
            glm::vec3 rotation = m_camera->getRotation();
            rotation.x += delta * mouseDelta.y * Settings::EditorCameraRotationSpeed;
            rotation.y -= delta * yawSign * mouseDelta.x * Settings::EditorCameraRotationSpeed;
            m_camera->setRotation(rotation);
            m_camera->setPosition(this->calculatePosition());
        }
    }
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
    return glm::quat(m_camera->getRotation());
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
