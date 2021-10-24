#include "EditorTargetOperation.hpp"

#include "aderite/rendering/PipelineState.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

EditorTargetOperation::EditorTargetOperation()
{}

void EditorTargetOperation::initialize() {
	TargetProvideOperation::initialize();
	editor::State::DebugRenderHandle = getHandle();
}

reflection::Type EditorTargetOperation::getType() const {
	return static_cast<reflection::Type>(reflection::EditorTypes::EditorTargetOp);
}

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
