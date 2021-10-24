#include "RenderNode.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/rendering/operation/RenderOperation.hpp"
#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/OperationArray.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

RenderNode::RenderNode()
{
    // Inputs
    p_inputs.push_back(new InputPin(
        this,
        std::string(node::getTypeName(node::PropertyType::Entity)) + "[]",
        "Objects"
    ));
    p_inputs.push_back(new InputPin(
        this,
        node::getTypeName(node::PropertyType::Eye),
        "Eye"
    ));
    p_inputs.push_back(new InputPin(
        this,
        node::getTypeName(node::PropertyType::Target),
        "Target"
    ));
    p_inputs.push_back(new InputPin(
        this,
        node::getTypeName(node::PropertyType::Require),
        "Require"
    ));

    // Outputs
    p_outputs.push_back(new OutputPin(
        this,
        node::getTypeName(node::PropertyType::Require),
        "Require"
    ));

    // Make require optional
    p_inputs[3]->setOptional(true);
}

void RenderNode::convertToArray() {
    p_inputs[1]->setType(std::string(node::getTypeName(node::PropertyType::Eye)) + "[]");
    p_inputs[2]->setType(std::string(node::getTypeName(node::PropertyType::Target)) + "[]");
}

void RenderNode::convertToSingle() {
    p_inputs[1]->setType(std::string(node::getTypeName(node::PropertyType::Eye)));
    p_inputs[2]->setType(std::string(node::getTypeName(node::PropertyType::Target)));
}

const char* RenderNode::getNodeName() const {
    return "Render";
}

void RenderNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);

    if (!m_array) {
        rendering::RenderOperation* op = new rendering::RenderOperation();
        pe->addOperation(op);
    }
    else {
        runtime::OperationArray* eyes = static_cast<runtime::OperationArray*>(pe->getOperation(p_inputs[1]->getValue()));
        runtime::OperationArray* targets = static_cast<runtime::OperationArray*>(pe->getOperation(p_inputs[2]->getValue()));

        rendering::EntityProvideOperation* entities = static_cast<rendering::EntityProvideOperation*>(pe->getOperation(p_inputs[0]->getValue()));

        if (eyes->size() != targets->size()) {
            LOG_ERROR("Incompatible sizes for eyes {0} and targets {1}", eyes->size(), targets->size());
        }
        else {
            for (size_t i = 0; i < eyes->size(); i++) {
                rendering::RenderOperation* op = new rendering::RenderOperation();
                pe->addOperation(op);
            }
        }
    }

    m_evaluated = true;
}

bool RenderNode::onConnectToInput(InputPin* target, OutputPin* source) {
    // Check which pin
    if (p_inputs[0] == target || p_inputs[3] == target) {
        // Entities or Require pin
        return true;
    }

    if (node::isArray(source->getType())) {
        if (!m_array) {
            // Convert to array
            m_array = true;
            convertToArray();
        }
    }
    else {
        if (m_array) {
            // Convert to single object
            m_array = false;
            convertToSingle();
        }
    }

    return true;
}

reflection::Type RenderNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::RenderNode);
}

bool RenderNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    emitter << YAML::Key << "IsArray" << YAML::Value << m_array;
    serializeData(emitter);
    return true;
}

bool RenderNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    m_array = data["IsArray"].as<bool>();
    if (m_array) {
        convertToArray();
    }
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

