#include "EditorRenderNode.hpp"

#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"

#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/runtime/EditorRenderOperation.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

namespace aderite {
namespace node {

EditorRenderNode::EditorRenderNode() {
    // Inputs
    p_inputs.push_back(new InputPin(this, node::getTypeName(node::PropertyType::Require), "Require"));

    // Output
    p_outputs.push_back(new OutputPin(this, node::getTypeName(node::PropertyType::Require), "Require"));
}

const char* EditorRenderNode::getNodeName() const {
    return "Editor render";
}

void EditorRenderNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    editor::EditorRenderOperation* op = new editor::EditorRenderOperation();
    pe->addOperation(op);
    m_evaluated = true;
}

reflection::Type EditorRenderNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::EditorRenderNode);
}

bool EditorRenderNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    serializeData(emitter);
    return true;
}

bool EditorRenderNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

} // namespace node
} // namespace aderite
