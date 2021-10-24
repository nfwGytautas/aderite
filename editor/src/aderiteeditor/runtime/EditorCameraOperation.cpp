#include "EditorCameraOperation.hpp"

#include "aderite/rendering/PipelineState.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

void EditorCameraOperation::execute(rendering::PipelineState* state) {
	state->pushEye({
		editor::State::EditorCamera->getViewMatrix(),
		editor::State::EditorCamera->getProjectionMatrix()
	});
}

reflection::Type EditorCameraOperation::getType() const {
	return static_cast<reflection::Type>(reflection::EditorTypes::EditorCameraOp);
}

bool EditorCameraOperation::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	return true;
}

bool EditorCameraOperation::deserialize(io::Serializer* serializer, const YAML::Node& data) {
	return true;
}

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
