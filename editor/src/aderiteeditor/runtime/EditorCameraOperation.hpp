#pragma once

#include "aderite/rendering/Forward.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

/**
 * @brief Operation used to provide an editor camera
*/
class EditorCameraOperation : public rendering::OperationBase {
public:
	// Inherited via EyeProvideOperation
	virtual void execute(rendering::PipelineState* state) override;
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
};

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
