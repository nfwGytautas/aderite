#include "AddNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

AddNode::AddNode(int id, Graph* graph, asset::prop::PropertyType type)
	: Node(id), m_type(type)
{
	// 2 inputs
	p_inputs.push_back(
		graph->createInputPin(this, asset::prop::getNameForType(type), "Input 1")
	);

	p_inputs.push_back(
		graph->createInputPin(this, asset::prop::getNameForType(type), "Input 2")
	);

	// 1 output
	p_outputs.push_back(
		graph->createOutputPin(this, asset::prop::getNameForType(type), "Output")
	);
}

void AddNode::setType(asset::prop::PropertyType type) {
	p_inputs[0]->setType(asset::prop::getNameForType(type));
	p_inputs[1]->setType(asset::prop::getNameForType(type));
	p_outputs[0]->setType(asset::prop::getNameForType(type));
	m_type = type;
}

const char* AddNode::getNodeName() const {
	return "Add";
}

void AddNode::renderBody() {
	// Change type dropdown
	std::string current = asset::prop::getNameForType(m_type);

	ImGui::PushItemWidth(80);
	if (ImGui::BeginCombo("Type", current.c_str(), 0)) {
		renderSelection(asset::prop::PropertyType::FLOAT);
		renderSelection(asset::prop::PropertyType::VEC2);
		renderSelection(asset::prop::PropertyType::VEC3);
		renderSelection(asset::prop::PropertyType::VEC4);
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
}

void AddNode::evaluate(compiler::GraphEvaluator* evaluator) {
	evaluateDependencies(evaluator);
	compiler::ShaderEvaluator* se = static_cast<compiler::ShaderEvaluator*>(evaluator);
	compiler::EvaluatorValue var = se->addAddInstruction(
		std::string(asset::prop::getNameForType(m_type)),
		p_inputs[0]->getValue(),
		p_inputs[1]->getValue()
	);
	p_outputs[0]->setValue(var);
	m_evaluated = true;
}

void AddNode::renderSelection(asset::prop::PropertyType type) {
	if (ImGui::Selectable(asset::prop::getNameForType(type))) {
		if (m_type != type) {
			setType(type);
		}
	}

	if (m_type == type) {
		ImGui::SetItemDefaultFocus();
	}
}

bool AddNode::serialize(YAML::Emitter& out) {
	out << YAML::BeginMap;
	out << YAML::Key << "NodeType" << YAML::Value << "Add";
	out << YAML::Key << "ElementType" << YAML::Value << static_cast<int>(m_type);
	serializeData(out);
	out << YAML::EndMap;
	return false;
}

bool AddNode::deserialize(YAML::Node& data) {
	setType(static_cast<asset::prop::PropertyType>(data["ElementType"].as<int>()));
	deserializeData(data);
	return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
