#include "InputPin.hpp"

#include "aderite/utility/Log.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/Link.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

InputPin::InputPin(int id, Node* node, const std::string& type, const std::string& name)
	: m_id(id), m_node(node), m_type(type), m_name(name)
{}

int InputPin::getId() const {
	return m_id;
}

std::string InputPin::getType() const {
	return m_type;
}

void InputPin::setType(const std::string& type) {
	m_type = type;
	disconnect();
}

std::string InputPin::getName() const {
	return m_name;
}

void InputPin::renderUI() {
	ImNodes::BeginInputAttribute(m_id);
	ImGui::Text(m_name.c_str());
	ImGui::SameLine();
	ImGui::Text(m_type.c_str());
	ImNodes::EndInputAttribute();
}

OutputPin* InputPin::getConnection() const {
	if (m_link == nullptr) {
		return nullptr;
	}

	return m_link->getOutputPin();
}

void InputPin::disconnect() {
	if (m_link) {
		m_link->destroy();
	}
}

compiler::Variable InputPin::getValue() const {
	OutputPin* pin = getConnection();
	if (!pin) {
		return "";
	}
	else {
		return pin->getValue();
	}
}

Node* InputPin::getNode() const {
	return m_node;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
