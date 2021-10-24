#include "SelectObjectNode.hpp"

#include <imgui/imgui.h>
#include "EntitiesNode.hpp"

#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

SelectObjectNode::SelectObjectNode()
{
    // Inputs
    p_inputs.push_back(new InputPin(
        this,
        "None[]",
        "Objects"
    ));

    // Output
    p_outputs.push_back(new OutputPin(
        this,
        "None",
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

bool SelectObjectNode::onConnectToInput(InputPin* target, OutputPin* source) {
    if (source->getType() != m_currentType) {
        this->setType(source->getType());
    }

    return true;
}

void SelectObjectNode::renderBody() {
    ImGui::InputScalar("Index", ImGuiDataType_U64, &m_index);
}

reflection::Type SelectObjectNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::ConcatObjectsNode);
}

bool SelectObjectNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    emitter << YAML::Key << "PinType" << YAML::Value << m_currentType;
    emitter << YAML::Key << "SelectIndex" << YAML::Value << m_index;
    serializeData(emitter);
    return true;
}

bool SelectObjectNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    if (m_currentType != data["PinType"].as<std::string>()) {
        this->setType(data["PinType"].as<std::string>());
    }
    m_index = data["SelectIndex"].as<size_t>();
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
