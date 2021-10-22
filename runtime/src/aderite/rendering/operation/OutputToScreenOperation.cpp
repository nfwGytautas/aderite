#include "OutputToScreenOperation.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

OutputToScreenOperation::OutputToScreenOperation() {}

void OutputToScreenOperation::execute(PipelineState* state) {

}

reflection::Type OutputToScreenOperation::getType() const {
	return static_cast<reflection::Type>(reflection::RuntimeTypes::OP_SCREEN);
}

bool OutputToScreenOperation::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	return true;
}

bool OutputToScreenOperation::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	return true;
}

ADERITE_RENDERING_NAMESPACE_END
