#include "RenderNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/RenderOperation.hpp"
#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderite/rendering/operation/EyeProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

RenderNode::RenderNode(int id, Graph* graph)
    : Node(id)
{
    // Inputs
    p_inputs.push_back(graph->createInputPin(
        this,
        std::string(pipeline::getTypeName(pipeline::PropertyType::Entity)) + "[]",
        "Objects"
    ));
    p_inputs.push_back(graph->createInputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Eye),
        "Eye"
    ));
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

    // Outputs
    p_outputs.push_back(graph->createOutputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Require),
        "Require"
    ));

    // Make require optional
    p_inputs[3]->setOptional(true);
}

const char* RenderNode::getNodeName() const {
    return "Render";
}

void RenderNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    rendering::RenderOperation* op = new rendering::RenderOperation(
        static_cast<rendering::EntityProvideOperation*>(pe->getOperation(p_inputs[0]->getValue())),
        static_cast<rendering::EyeProvideOperation*>(pe->getOperation(p_inputs[1]->getValue())),
        static_cast<rendering::TargetProvideOperation*>(pe->getOperation(p_inputs[2]->getValue()))
    );
    pe->addOperation(op);
    m_evaluated = true;
}

bool RenderNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "Render";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool RenderNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

