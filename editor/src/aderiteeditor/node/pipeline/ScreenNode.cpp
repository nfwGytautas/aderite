#include "ScreenNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/OutputToScreenOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

ScreenNode::ScreenNode(int id, Graph* graph)
    : Node(id)
{
    p_inputs.push_back(graph->createInputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Target),
        "Target"
    ));

    p_inputs.push_back(graph->createInputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Require),
        "Require"
    ));

    // Make require optional
    p_inputs[1]->setOptional(true);
}

const char* ScreenNode::getNodeName() const {
    return "Screen";
}

void ScreenNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    rendering::OutputToScreenOperation* op = new rendering::OutputToScreenOperation(
        static_cast<rendering::TargetProvideOperation*>(pe->getOperation(p_inputs[0]->getValue()))
    );
    pe->addOperation(op);
    m_evaluated = true;
}

bool ScreenNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "Screen";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool ScreenNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

