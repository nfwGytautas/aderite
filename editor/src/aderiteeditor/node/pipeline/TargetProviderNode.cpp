#include "TargetProviderNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

TargetProviderNode::TargetProviderNode(int id, Graph* graph)
    : Node(id)
{
    p_outputs.push_back(graph->createOutputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Target),
        "Target"
    ));
}

const char* TargetProviderNode::getNodeName() const {
    return "Target provider";
}

void TargetProviderNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    rendering::TargetProvideOperation* op = new rendering::TargetProvideOperation();
    p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

bool TargetProviderNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "Target";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool TargetProviderNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

