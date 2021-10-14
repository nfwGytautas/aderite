#include "EditorCameraOperation.hpp"

#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

EditorCameraOperation::EditorCameraOperation()
{
	p_camera = shared::State::EditorCamera;
}

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
