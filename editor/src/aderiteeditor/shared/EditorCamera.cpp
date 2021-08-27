#include "EditorCamera.hpp"

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/utility/bgfx.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/Aderite.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Settings.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

EditorCamera::EditorCamera() {
	// Create framebuffer
	m_target = utility::createFrameBuffer();

	updateViewMatrix();
}

EditorCamera::~EditorCamera() {
	bgfx::destroy(m_target);
}

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

bgfx::FrameBufferHandle EditorCamera::getOutputHandle() {
	return m_target;
}

glm::mat4 EditorCamera::computeViewMatrix() {
	return m_viewMatrix;
}

glm::mat4 EditorCamera::computeProjectionMatrix() {
	return m_projectionMatrix;
}

void EditorCamera::update(float delta) {
	// TODO: Moving, zoom
	bool wasUpdated = false;

	if (::aderite::Engine::getInputManager()->isMouseKeyPressed(input::MouseKey::MIDDLE)) {
		// Rotate
		glm::vec2 mouseDelta = ::aderite::Engine::getInputManager()->getMouseDelta();

		float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
		m_yaw += delta * yawSign * mouseDelta.x * 0.8f;
		m_pitch += delta * mouseDelta.y * 0.8f;

		wasUpdated = true;
	}

	if (wasUpdated) {
		updateViewMatrix();
	}
}

bool EditorCamera::isEnabled() {
	return !State::IsGameMode;
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
	return glm::quat(glm::vec3(-m_pitch, -m_yaw, 0.0f));
}

void EditorCamera::updateViewMatrix() {
	glm::quat orientation = getOrientation();
	m_viewMatrix = glm::translate(glm::mat4(1.0f), calculatePosition()) * glm::toMat4(orientation);
	m_viewMatrix = glm::inverse(m_viewMatrix);
}

ADERITE_EDITOR_SHARED_NAMESPACE_END
