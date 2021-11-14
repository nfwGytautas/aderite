#include "ConcatObjectsNode.hpp"

#include "aderite/rendering/operation/EntityProvideOperation.hpp"

#include "EntitiesNode.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/runtime/OperationArray.hpp"

namespace aderite {
namespace node {

ConcatObjectsNode::ConcatObjectsNode() {
    // Inputs
    p_inputs.push_back(new InputPin(this, "Object", "Object 1"));

    p_inputs.push_back(new InputPin(this, "Object", "Object 1"));

    // TODO: Add more dynamically

    // Output
    p_outputs.push_back(new OutputPin(this, "Object[]", "Array"));
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
    editor::OperationArray* op = new editor::OperationArray({});
    op->setName("Concat " + m_currentType);
    for (InputPin* ipin : p_inputs) {
        op->addOperation(pe->getOperation(ipin->getValue()));
    }
    p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

bool ConcatObjectsNode::onConnectToInput(InputPin* target, OutputPin* source) {
    if (source->getType() != m_currentType) {
        this->setType(source->getType());
    }

    return true;
}

reflection::Type ConcatObjectsNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::ConcatObjectsNode);
}

bool ConcatObjectsNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "PinType" << YAML::Value << m_currentType;
    serializeData(emitter);
    return true;
}

bool ConcatObjectsNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    if (m_currentType != data["PinType"].as<std::string>()) {
        this->setType(data["PinType"].as<std::string>());
    }
    return true;
}

} // namespace node
} // namespace aderite
