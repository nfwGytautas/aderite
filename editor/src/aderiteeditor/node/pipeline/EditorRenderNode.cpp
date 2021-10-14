#include "EditorRenderNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/EditorRenderOperation.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

EditorRenderNode::EditorRenderNode(int id, Graph* graph)
    : Node(id)
{
    // Inputs
    p_inputs.push_back(graph->createInputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Require),
        "Require"
    ));

    // Output
    p_outputs.push_back(graph->createOutputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Require),
        "Require"
    ));
}

const char* EditorRenderNode::getNodeName() const {
    return "Editor render";
}

void EditorRenderNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    runtime::EditorRenderOperation* op = new runtime::EditorRenderOperation();
    pe->addOperation(op);
    m_evaluated = true;
}

bool EditorRenderNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "EditorRender";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool EditorRenderNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
