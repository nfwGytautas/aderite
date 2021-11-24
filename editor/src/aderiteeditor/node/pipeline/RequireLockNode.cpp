#include "RequireLockNode.hpp"

#include "aderite/rendering/operation/EntityProvideOperation.hpp"

#include "EntitiesNode.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

namespace aderite {
namespace node {

RequireLockNode::RequireLockNode() {
    // Inputs
    p_inputs.push_back(new InputPin(this, std::string(node::getTypeName(node::PropertyType::Require)), "Require"));

    p_inputs.push_back(new InputPin(this, std::string(node::getTypeName(node::PropertyType::Require)), "Require"));

    // TODO: Add more dynamically

    // Output
    p_outputs.push_back(new OutputPin(this, std::string(node::getTypeName(node::PropertyType::Require)), "Require"));
}

const char* RequireLockNode::getNodeName() const {
    return "Require lock";
}

void RequireLockNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    m_evaluated = true;
}

reflection::Type RequireLockNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::ConcatObjectsNode);
}

bool RequireLockNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    serializeData(emitter);
    return true;
}

bool RequireLockNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

} // namespace node
} // namespace aderite
