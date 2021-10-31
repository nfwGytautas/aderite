#include "ConvertNode.hpp"

#include "aderite/rendering/operation/All.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/runtime/OperationArray.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

namespace aderite {
namespace node {

ConvertNode::ConvertNode() {
    p_inputs.push_back(new InputPin(this, "None", "From"));

    p_outputs.push_back(new OutputPin(this, "None", "To"));
}

void ConvertNode::setFromType(const std::string& from) {
    p_inputs[0]->setType(from);

    // TODO: Add convenience function for converting to and from arrays
    if (node::isArray(from) && !node::isArray(p_outputs[0]->getType())) {
        // Convert output to array
        p_outputs[0]->setType(p_outputs[0]->getType() + "[]");
    } else if (!node::isArray(from) && node::isArray(p_outputs[0]->getType())) {
        // Remove array
        p_outputs[0]->setType(p_outputs[0]->getType().substr(0, p_outputs[0]->getType().length() - 2));
    }
}

void ConvertNode::setToType(const std::string& to) {
    p_outputs[0]->setType(to);

    if (node::isArray(to) && !node::isArray(p_inputs[0]->getType())) {
        // Convert output to array
        p_inputs[0]->setType(p_inputs[0]->getType() + "[]");
    } else if (!node::isArray(to) && node::isArray(p_inputs[0]->getType())) {
        // Remove array
        p_inputs[0]->setType(p_inputs[0]->getType().substr(0, p_inputs[0]->getType().length() - 2));
    }
}

const char* ConvertNode::getNodeName() const {
    return "Convert";
}

void ConvertNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);

    compiler::PipelineEvaluator* pe = dynamic_cast<compiler::PipelineEvaluator*>(evaluator);
    if (pe != nullptr) {
        handlePipelineConvert(pe);
    } else {
        handleShaderConvert(dynamic_cast<compiler::ShaderEvaluator*>(evaluator));
    }

    m_evaluated = true;
}

bool ConvertNode::onConnectToInput(InputPin* target, OutputPin* source) {
    if (source->getType() != p_inputs[0]->getType()) {
        this->setFromType(source->getType());
    }

    return true;
}

bool ConvertNode::onConnectToOutput(OutputPin* target, InputPin* source) {
    if (source->getType() != p_outputs[0]->getType()) {
        this->setToType(source->getType());
    }

    return true;
}

reflection::Type ConvertNode::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::ConvertNode);
}

bool ConvertNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    emitter << YAML::Key << "From" << YAML::Value << p_inputs[0]->getType();
    emitter << YAML::Key << "To" << YAML::Value << p_outputs[0]->getType();
    serializeData(emitter);
    return true;
}

bool ConvertNode::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    deserializeData(data);
    p_inputs[0]->setType(data["From"].as<std::string>());
    p_outputs[0]->setType(data["To"].as<std::string>());
    return true;
}

void ConvertNode::handlePipelineConvert(compiler::PipelineEvaluator* pe) {
    LOG_ERROR("Unknown conversion from {0} to {1}", p_inputs[0]->getType(), p_outputs[0]->getType());
}

void ConvertNode::handleShaderConvert(compiler::ShaderEvaluator* se) {}

void ConvertNode::convert(compiler::PipelineEvaluator* pe, PipelineConversionFn cfn) {
    if (node::isArray(p_inputs[0]->getType())) {
        // Array convert
        applyArray(pe, cfn);
    } else {
        rendering::OperationBase* from = pe->getOperation(p_inputs[0]->getValue());
        rendering::OperationBase* to = (this->*cfn)(from);
        p_outputs[0]->setValue(pe->addOperation(to));
    }
}

void ConvertNode::applyArray(compiler::PipelineEvaluator* pe, PipelineConversionFn cfn) {
    editor::OperationArray* oparr = static_cast<editor::OperationArray*>(pe->getOperation(p_inputs[0]->getValue()));
    editor::OperationArray* result = new editor::OperationArray();
    for (rendering::OperationBase* op : *oparr) {
        rendering::OperationBase* converted = (this->*cfn)(op);
        pe->addOperation(converted);
        result->addOperation(converted);
    }
    result->setName("Convert (" + p_inputs[0]->getType() + "->" + p_outputs[0]->getType() + ")");
    p_outputs[0]->setValue(pe->addOperation(result));
}

} // namespace node
} // namespace aderite
