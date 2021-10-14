#include "RenderNode.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/RenderOperation.hpp"
#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderite/rendering/operation/EyeProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/runtime/OperationArray.hpp"

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

void RenderNode::convertToArray() {
    p_inputs[1]->setType(std::string(pipeline::getTypeName(pipeline::PropertyType::Eye)) + "[]");
    p_inputs[2]->setType(std::string(pipeline::getTypeName(pipeline::PropertyType::Target)) + "[]");
}

void RenderNode::convertToSingle() {
    p_inputs[1]->setType(std::string(pipeline::getTypeName(pipeline::PropertyType::Eye)));
    p_inputs[2]->setType(std::string(pipeline::getTypeName(pipeline::PropertyType::Target)));
}

const char* RenderNode::getNodeName() const {
    return "Render";
}

void RenderNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);

    if (!m_array) {
        rendering::RenderOperation* op = new rendering::RenderOperation(
            static_cast<rendering::EntityProvideOperation*>(pe->getOperation(p_inputs[0]->getValue())),
            static_cast<rendering::EyeProvideOperation*>(pe->getOperation(p_inputs[1]->getValue())),
            static_cast<rendering::TargetProvideOperation*>(pe->getOperation(p_inputs[2]->getValue()))
        );
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
                rendering::RenderOperation* op = new rendering::RenderOperation(
                    entities,
                    static_cast<rendering::EyeProvideOperation*>((*eyes)[i]),
                    static_cast<rendering::TargetProvideOperation*>((*targets)[i])
                );
                pe->addOperation(op);
            }
        }
    }

    m_evaluated = true;
}

bool RenderNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "Render";
    out << YAML::Key << "IsArray" << YAML::Value << m_array;
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool RenderNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    m_array = data["IsArray"].as<bool>();
    if (m_array) {
        convertToArray();
    }
    return true;
}

bool RenderNode::onConnectToInput(InputPin* target, OutputPin* source) {
    // Check which pin
    if (p_inputs[0] == target || p_inputs[3] == target) {
        // Entities or Require pin
        return true;
    }

    if (pipeline::isArray(source->getType())) {
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

ADERITE_EDITOR_NODE_NAMESPACE_END

