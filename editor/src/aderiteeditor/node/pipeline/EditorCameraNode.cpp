#include "EditorCameraNode.hpp"

#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/EditorCameraOperation.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

EditorCameraNode::EditorCameraNode()
{
    // Output
    p_outputs.push_back(new OutputPin(
        this,
        node::getTypeName(node::PropertyType::Eye),
        "Eye"
    ));
}

const char* EditorCameraNode::getNodeName() const {
    return "Editor camera";
}

void EditorCameraNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    runtime::EditorCameraOperation* op = new runtime::EditorCameraOperation();
    p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

reflection::Type EditorCameraNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::EditorCameraNode);
}

bool EditorCameraNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    serializeData(emitter);
    return true;
}

bool EditorCameraNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
