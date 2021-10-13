#pragma once

#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to render color and depth to a target
*/
class RenderOperation : public OperationBase {
public:
	RenderOperation(EntityProvideOperation* entities, EyeProvideOperation* eye, TargetProvideOperation* target);

	// Inherited via OperationBase
	virtual void execute() override;
private:
	EntityProvideOperation* m_entities = nullptr;
	EyeProvideOperation* m_eye = nullptr;
	TargetProvideOperation* m_target = nullptr;
};

ADERITE_RENDERING_NAMESPACE_END
