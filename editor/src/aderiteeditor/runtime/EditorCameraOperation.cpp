#include "EditorCameraOperation.hpp"

#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

const glm::mat4& EditorCameraOperation::getViewMatrix() const {
	return editor::State::EditorCamera->getViewMatrix();
}

const glm::mat4& EditorCameraOperation::getProjectionMatrix() const {
	return editor::State::EditorCamera->getProjectionMatrix();
}

bool EditorCameraOperation::isValid() const {
	return editor::State::EditorCamera->isEnabled();
}

ADERITE_EDITOR_RUNTIME_NAMESPACE_END


