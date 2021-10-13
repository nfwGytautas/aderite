#pragma once

#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to output a target to screen
*/
class OutputToScreenOperation : public OperationBase {
public:
	OutputToScreenOperation(TargetProvideOperation* target);

	// Inherited via OperationBase
	virtual void execute() override;

private:
	TargetProvideOperation* m_target;
};

ADERITE_RENDERING_NAMESPACE_END
