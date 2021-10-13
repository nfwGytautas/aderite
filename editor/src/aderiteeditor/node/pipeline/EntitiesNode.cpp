#include "EntitiesNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

EntitiesNode::EntitiesNode(int id, Graph* graph)
    : Node(id)
{
    p_outputs.push_back(graph->createOutputPin(
        this,
        std::string(pipeline::getTypeName(pipeline::PropertyType::Entity)) + "[]",
        "Entities"
    ));
}

const char* EntitiesNode::getNodeName() const {
    return "Entities";
}

void EntitiesNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    rendering::EntityProvideOperation* op = new rendering::EntityProvideOperation();
    p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

bool EntitiesNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "Entities";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool EntitiesNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

