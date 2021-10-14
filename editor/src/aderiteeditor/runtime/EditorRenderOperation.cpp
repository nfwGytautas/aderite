#include "EditorRenderOperation.hpp"

#include <bgfx/bgfx.h>
#include "aderite/utility/Log.hpp"
#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

EditorRenderOperation::EditorRenderOperation()
{
	p_debugName = "EditorRenderHook";
}

EditorRenderOperation::~EditorRenderOperation() {

}

void EditorRenderOperation::initialize() {
	ADERITE_DEBUG_SECTION
	(
		bgfx::setViewName(c_ViewId, p_debugName.c_str());
	)
}

void EditorRenderOperation::execute() {

}

void EditorRenderOperation::shutdown() {

}

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
