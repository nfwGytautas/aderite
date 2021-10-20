#pragma once

#include "aderite/rendering/operation/EyeProvideOperation.hpp"
#include "aderite/rendering/operation/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

/**
 * @brief Operation used to provide an editor camera
*/
class EditorCameraOperation : public rendering::EyeProvideOperation {
public:
	EditorCameraOperation();

	// Inherited via EyeProvideOperation
	virtual const glm::mat4& getViewMatrix() const override;
	virtual const glm::mat4& getProjectionMatrix() const override;
	virtual bool isValid() const override;

	ADERITE_DEBUG_SECTION
	(
		virtual const char* getOperationName() override { return "EditorCameraOperation"; }
	);
};

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
