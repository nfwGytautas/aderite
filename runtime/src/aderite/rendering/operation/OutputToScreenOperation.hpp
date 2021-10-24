#pragma once

#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to output a target to screen
*/
class OutputToScreenOperation : public OperationBase {
public:
	OutputToScreenOperation();

	// Inherited via OperationBase
	virtual void execute(PipelineState* state) override;
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

ADERITE_RENDERING_NAMESPACE_END
