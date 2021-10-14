#include "RequireLockNode.hpp"

#include "EntitiesNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

RequireLockNode::RequireLockNode(int id, Graph* graph)
    : Node(id)
{
    // Inputs
    p_inputs.push_back(graph->createInputPin(
        this,
        std::string(pipeline::getTypeName(pipeline::PropertyType::Require)),
        "Require"
    ));

    p_inputs.push_back(graph->createInputPin(
        this,
        std::string(pipeline::getTypeName(pipeline::PropertyType::Require)),
        "Require"
    ));

    // TODO: Add more dynamically

    // Output
    p_outputs.push_back(graph->createOutputPin(
        this,
        std::string(pipeline::getTypeName(pipeline::PropertyType::Require)),
        "Require"
    ));
}

const char* RequireLockNode::getNodeName() const {
    return "Require lock";
}

void RequireLockNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    m_evaluated = true;
}

bool RequireLockNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "RequireLock";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool RequireLockNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
