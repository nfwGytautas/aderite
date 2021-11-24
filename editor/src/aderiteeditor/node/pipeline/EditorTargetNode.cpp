#include "EditorTargetNode.hpp"

#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/runtime/EditorTargetOperation.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

namespace aderite {
namespace node {

EditorTargetNode::EditorTargetNode() {
    // Output
    p_outputs.push_back(new OutputPin(this, node::getTypeName(node::PropertyType::Target), "Target"));
}

const char* EditorTargetNode::getNodeName() const {
    return "Editor target";
}

void EditorTargetNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    editor::EditorTargetOperation* op = new editor::EditorTargetOperation();
    p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

reflection::Type EditorTargetNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::EditorTargetNode);
}

bool EditorTargetNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    serializeData(emitter);
    return true;
}

bool EditorTargetNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

} // namespace node
} // namespace aderite
