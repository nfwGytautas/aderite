#pragma once

#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to create an entity provider
*/
class EntityProvideOperation : public OperationBase {
public:
	// Inherited via OperationBase
	virtual void execute() override;
};

ADERITE_RENDERING_NAMESPACE_END
