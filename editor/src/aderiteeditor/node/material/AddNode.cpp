#include "AddNode.hpp"

#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/runtime/EditorTypes.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

AddNode::AddNode()
{
	// 2 inputs
	p_inputs.push_back(
		new InputPin(this, "None", "Input 1")
	);

	p_inputs.push_back(
		new InputPin(this, "None", "Input 2")
	);

	// 1 output
	p_outputs.push_back(
		new OutputPin(this, "None", "Output")
	);
}

void AddNode::setType(std::string type) {
	p_inputs[0]->setType(type);
	p_inputs[1]->setType(type);
	p_outputs[0]->setType(type);
	m_type = type;
}

const char* AddNode::getNodeName() const {
	return "Add";
}

void AddNode::evaluate(compiler::GraphEvaluator* evaluator) {
	evaluateDependencies(evaluator);
	compiler::ShaderEvaluator* se = static_cast<compiler::ShaderEvaluator*>(evaluator);
	compiler::EvaluatorValue var = se->addAddInstruction(
		std::string(m_type),
		p_inputs[0]->getValue(),
		p_inputs[1]->getValue()
	);
	p_outputs[0]->setValue(var);
	m_evaluated = true;
}

reflection::Type AddNode::getType() const {
	return static_cast<reflection::Type>(reflection::EditorTypes::AddNode);
}

bool AddNode::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	emitter << YAML::Key << "ElementType" << YAML::Value << m_type;
	serializeData(emitter);
	return true;
}

bool AddNode::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	setType(data["ElementType"].as<std::string>());
	deserializeData(data);
	return true;
}

bool AddNode::onConnectToInput(InputPin* target, OutputPin* source) {
	if (source->getType() != m_type) {
		setType(source->getType());
	}

	return true;
}

bool AddNode::onConnectToOutput(OutputPin* target, InputPin* source) {
	if (source->getType() != m_type) {
		setType(source->getType());
	}

	return false;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
