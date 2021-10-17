#include "Node.hpp"

#include "aderite/utility/Log.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

Node::Node() {}


Node::Node(int id)
	: m_id(id)
{}

Node::~Node() {
	for (OutputPin* p : p_outputs) {
		delete p;
	}

	for (InputPin* p : p_inputs) {
		delete p;
	}
}

int Node::getId() const {
	return m_id;
}

void Node::setId(int id) {
	m_id = id;
}

std::vector<InputPin*> Node::getInputPins() const {
	return p_inputs;
}

std::vector<OutputPin*> Node::getOutputPins() const {
	return p_outputs;
}

void Node::renderUI() {
	
}

void Node::evaluate(compiler::GraphEvaluator* evaluator) {
	m_evaluated = true;
}

void Node::evaluateDependencies(compiler::GraphEvaluator* evaluator) {
	for (InputPin* pin : p_inputs) {
		OutputPin* opin = pin->getConnection();
		if (opin == nullptr) {
			if (!pin->isOptional()) {
				LOG_ERROR("Input pin without connected output pin");
			}
			continue;
		}

		Node* n = opin->getNode();
		if (!n->m_evaluated) {
			n->evaluate(evaluator);
		}
	}
}

void Node::resetEvaluateFlag() {
	m_evaluated = false;
}

void Node::serializeData(YAML::Emitter& out) {
	out << YAML::Key << "InputPins" << YAML::Flow << YAML::BeginSeq;
	for (InputPin* pin : getInputPins()) {
		out << pin->getId();
	}
	out << YAML::EndSeq;

	out << YAML::Key << "OutputPins" << YAML::Flow << YAML::BeginSeq;
	for (OutputPin* pin : getOutputPins()) {
		out << pin->getId();
	}
	out << YAML::EndSeq;

	out << YAML::Key << "Position" << YAML::BeginMap;
	ImVec2 pos = ImNodes::GetNodeGridSpacePos(m_id);
	out << YAML::Key << "X" << YAML::Value << pos.x;
	out << YAML::Key << "Y" << YAML::Value << pos.y;
	out << YAML::EndMap;

	out << YAML::Key << "ID" << YAML::Value << m_id;
}

void Node::deserializeData(const YAML::Node& data) {
	m_id = data["ID"].as<int>();

	size_t idx = 0;
	for (const YAML::Node& inpin : data["InputPins"]) {
		p_inputs[idx]->m_id = inpin.as<int>();
		idx++;
	}
	idx = 0;
	for (const YAML::Node& outpin : data["OutputPins"]) {
		p_outputs[idx]->m_id = outpin.as<int>();
		idx++;
	}

	ImVec2 position;
	position.x = data["Position"]["X"].as<float>();
	position.y = data["Position"]["Y"].as<float>();
	ImNodes::SetNodeGridSpacePos(m_id, position);
}

ADERITE_EDITOR_NODE_NAMESPACE_END
