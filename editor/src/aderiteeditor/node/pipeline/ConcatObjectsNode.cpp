#include "ConcatObjectsNode.hpp"

#include "EntitiesNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/OperationArray.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

ConcatObjectsNode::ConcatObjectsNode(int id, Graph* graph, const std::string& type)
    : Node(id), m_currentType(type)
{
    // Inputs
    p_inputs.push_back(graph->createInputPin(
        this,
        type,
        "Object 1"
    ));

    p_inputs.push_back(graph->createInputPin(
        this,
        type,
        "Object 1"
    ));

    // TODO: Add more dynamically

    // Output
    p_outputs.push_back(graph->createOutputPin(
        this,
        type + "[]",
        "Array"
    ));
}

void ConcatObjectsNode::setType(const std::string& newType) {
    m_currentType = newType;
    p_outputs[0]->setType(newType + "[]");

    // Disconnect all previous pins
    for (InputPin* ipin : p_inputs) {
        ipin->setType(newType);
    }
}

const char* ConcatObjectsNode::getNodeName() const {
    return "Concat";
}

void ConcatObjectsNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    runtime::OperationArray* op = new runtime::OperationArray({});
    op->setDebugName("Concat " + m_currentType);
    for (InputPin* ipin : p_inputs) {
        op->addOperation(pe->getOperation(ipin->getValue()));
    }
    p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

bool ConcatObjectsNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "ConcatObjects";
    out << YAML::Key << "PinType" << YAML::Value << m_currentType;
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool ConcatObjectsNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    if (m_currentType != data["PinType"].as<std::string>()) {
        this->setType(data["PinType"].as<std::string>());
    }
    return true;
}

bool ConcatObjectsNode::onConnectToInput(InputPin* target, OutputPin* source) {
    if (source->getType() != m_currentType) {
        this->setType(source->getType());
    }

    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
