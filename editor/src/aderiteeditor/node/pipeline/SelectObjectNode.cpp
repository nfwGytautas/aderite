#include "SelectObjectNode.hpp"

#include <imgui/imgui.h>
#include "EntitiesNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

SelectObjectNode::SelectObjectNode(int id, Graph* graph, const std::string& type)
    : Node(id), m_currentType(type)
{
    // Inputs
    p_inputs.push_back(graph->createInputPin(
        this,
        type + "[]",
        "Objects"
    ));

    // Output
    p_outputs.push_back(graph->createOutputPin(
        this,
        type,
        "Item"
    ));
}

void SelectObjectNode::setType(const std::string& newType) {
    m_currentType = newType;
    p_inputs[0]->setType(newType + "[]");
    p_outputs[0]->setType(newType);
}

const char* SelectObjectNode::getNodeName() const {
    return "Select";
}

void SelectObjectNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    //rendering::EntityProvideOperation* op = new rendering::EntityProvideOperation();
    //p_outputs[0]->setValue(pe->addOperation(op));
    m_evaluated = true;
}

bool SelectObjectNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "SelectObject";
    out << YAML::Key << "PinType" << YAML::Value << m_currentType;
    out << YAML::Key << "SelectIndex" << YAML::Value << m_index;
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool SelectObjectNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    if (m_currentType != data["PinType"].as<std::string>()) {
        this->setType(data["PinType"].as<std::string>());
    }
    m_index = data["SelectIndex"].as<size_t>();
    return true;
}

bool SelectObjectNode::onConnectToInput(InputPin* target, OutputPin* source) {
    if (source->getType() != m_currentType) {
        this->setType(source->getType());
    }

    return true;
}

void SelectObjectNode::renderBody() {
    ImGui::InputScalar("Index", ImGuiDataType_U64, &m_index);
}

ADERITE_EDITOR_NODE_NAMESPACE_END
