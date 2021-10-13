#include "EyeProvideOperation.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/rendering/operation/CameraProvideOperation.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

void EyeProvideOperation::provideFromCamera(CameraProvideOperation* camera) {
	m_camera = camera;
}

const glm::mat4& EyeProvideOperation::getViewMatrix() const {
	return m_viewMatrix;
}

const glm::mat4& EyeProvideOperation::getProjectionMatrix() const {
	return m_projectionMatrix;
}

bool EyeProvideOperation::isValid() const {
	return m_camera != nullptr && m_camera->getCamera() != nullptr;
}

void EyeProvideOperation::execute() {
	if (isValid()) {
		m_viewMatrix = m_camera->getCamera()->computeViewMatrix();
		m_projectionMatrix = m_camera->getCamera()->computeProjectionMatrix();
	}
}

ADERITE_RENDERING_NAMESPACE_END
