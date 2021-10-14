#include "EditorTargetNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/EditorTargetOperation.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

EditorTargetNode::EditorTargetNode(int id, Graph* graph)
    : Node(id)
{
    // Output
    p_outputs.push_back(graph->createOutputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Target),
        "Target"
    ));
}

const char* EditorTargetNode::getNodeName() const {
    return "Editor target";
}

void EditorTargetNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    runtime::EditorTargetOperation* op = new runtime::EditorTargetOperation();
    pe->addOperation(op);
    m_evaluated = true;
}

bool EditorTargetNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "EditorTarget";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool EditorTargetNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
