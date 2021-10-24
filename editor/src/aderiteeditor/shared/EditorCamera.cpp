#include "EditorCamera.hpp"

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/utility/Log.hpp"
#include "aderite/Aderite.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Settings.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

EditorCamera::EditorCamera() {
	updateViewMatrix();
}

EditorCamera::~EditorCamera() {}

void EditorCamera::onViewportResize(const glm::uvec2& size) {
	if (size.x == 0 || size.y == 0) {
		return;
	}

	m_viewportSize = size;
	m_projectionMatrix = glm::perspective(
		glm::radians(Settings::EditorCameraFov),
		(float)m_viewportSize.x / (float)m_viewportSize.y,
		0.1f,
		1000.0f);
}

const glm::mat4& EditorCamera::getViewMatrix() const {
	return m_viewMatrix;
}

const glm::mat4& EditorCamera::getProjectionMatrix() const {
	return m_projectionMatrix;
}

void EditorCamera::update(float delta) {
	bool wasUpdated = false;

	auto inputManager = ::aderite::Engine::getInputManager();

	if (inputManager->isKeyPressed(input::Key::LEFT_ALT)) {
		// Zoom
		double scrollDelta = inputManager->getScrollDelta();

		if (scrollDelta != 0.0) {
			m_distance -= delta * scrollDelta * zoomSpeed();
			if (m_distance < 1.0f) 			{
				m_focalpoint += getForwardDirection();
				m_distance = 1.0f;
			}

			wasUpdated = true;
		}

		// Pan
		if (inputManager->isMouseKeyPressed(input::MouseKey::MIDDLE)) {
			glm::vec2 mouseDelta = inputManager->getMouseDelta();

			glm::vec2 speed = panSpeed();
			m_focalpoint += -getRightDirection() * mouseDelta.x * speed.x * m_distance * delta;
			m_focalpoint += getUpDirection() * mouseDelta.y * speed.y * m_distance * delta;

			wasUpdated = true;
		}
	}
	else {
		// Rotate
		if (inputManager->isMouseKeyPressed(input::MouseKey::MIDDLE)) {
			glm::vec2 mouseDelta = inputManager->getMouseDelta();

			float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
			m_yaw += delta * yawSign * mouseDelta.x * Settings::EditorCameraRotationSpeed;
			m_pitch += delta * mouseDelta.y * Settings::EditorCameraRotationSpeed;

			wasUpdated = true;
		}
	}

	if (wasUpdated) {
		updateViewMatrix();
	}
}

bool EditorCamera::isEnabled() const {
	//return !State::IsGameMode;
	return true;
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
	return glm::quat(glm::vec3(m_pitch, m_yaw, 0.0f));
}

void EditorCamera::updateViewMatrix() {
	glm::quat orientation = getOrientation();
	m_viewMatrix = glm::translate(glm::mat4(1.0f), calculatePosition()) * glm::toMat4(orientation);
	m_viewMatrix = glm::inverse(m_viewMatrix);
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

	return { xFactor, yFactor };
}

ADERITE_EDITOR_SHARED_NAMESPACE_END
