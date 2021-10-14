#include "EditorTargetOperation.hpp"

#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

EditorTargetOperation::EditorTargetOperation()
{}

void EditorTargetOperation::initialize() {
	TargetProvideOperation::initialize();
	shared::State::DebugRenderHandle = getHandle();
}

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
