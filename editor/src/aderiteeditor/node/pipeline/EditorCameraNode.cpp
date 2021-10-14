#include "EditorCameraNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/EditorCameraOperation.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

EditorCameraNode::EditorCameraNode(int id, Graph* graph)
    : Node(id)
{
    // Output
    p_outputs.push_back(graph->createOutputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Camera),
        "Camera"
    ));
}

const char* EditorCameraNode::getNodeName() const {
    return "Editor camera";
}

void EditorCameraNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    runtime::EditorCameraOperation* op = new runtime::EditorCameraOperation();
    pe->addOperation(op);
    m_evaluated = true;
}

bool EditorCameraNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "EditorCamera";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool EditorCameraNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
