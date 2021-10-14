#pragma once

#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderite/rendering/operation/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

/**
 * @brief Operation used to provide editor target
*/
class EditorTargetOperation : public rendering::TargetProvideOperation {
public:
	EditorTargetOperation();

	// Inherited from TargetProvideOperation
	virtual void initialize() override;
};

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
