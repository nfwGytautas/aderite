#include "OutputPin.hpp"

#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/Link.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

OutputPin::OutputPin(int id, Node* node, const std::string& type, const std::string& name)
	: m_id(id), m_node(node), m_type(type), m_name(name)
{}

int OutputPin::getId() const {
	return m_id;
}

std::string OutputPin::getType() const {
	return m_type;
}

void OutputPin::setType(const std::string& type) {
	m_type = type;
	disconnect();
}

std::string OutputPin::getName() const {
	return m_name;
}

void OutputPin::disconnect() {
	for (Link* l : m_links) {
		l->destroy();
	}

	m_links.clear();
}

void OutputPin::renderUI() {
	ImNodes::BeginOutputAttribute(m_id);
	ImGui::Text(m_name.c_str());
	ImGui::SameLine();
	ImGui::Text(m_type.c_str());
	ImNodes::EndOutputAttribute();
}

void OutputPin::setValue(compiler::Variable value) {
	m_value = value;
}

compiler::Variable OutputPin::getValue() const {
	return m_value;
}

Node* OutputPin::getNode() const {
	return m_node;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
