#include "OutputToScreenOperation.hpp"

namespace aderite {
namespace rendering {

OutputToScreenOperation::OutputToScreenOperation() {}

void OutputToScreenOperation::execute(PipelineState* state) {}

reflection::Type OutputToScreenOperation::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::OP_SCREEN);
}

bool OutputToScreenOperation::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    return true;
}

bool OutputToScreenOperation::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    return true;
}

} // namespace rendering
} // namespace aderite
