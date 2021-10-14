#pragma once

#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderite/rendering/operation/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

/**
 * @brief Operation used to provide an editor camera
*/
class EditorCameraOperation : public rendering::CameraProvideOperation {
public:
	EditorCameraOperation();

	ADERITE_DEBUG_SECTION
	(
		virtual const char* getOperationName() override { return "EditorCameraOperation"; }
	)
};

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
