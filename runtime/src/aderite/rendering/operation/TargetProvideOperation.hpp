#pragma once

#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to provide a rendering target
*/
class TargetProvideOperation : public OperationBase {
public:
	// Inherited via OperationBase
	virtual void execute() override;
};

ADERITE_RENDERING_NAMESPACE_END
