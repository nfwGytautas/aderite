#include "InputPin.hpp"

#include "aderite/utility/Log.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

InputPin::InputPin(int id, const std::string& type, const std::string& name)
	: m_id(id), m_type(type), m_name(name)
{}

int InputPin::getId() const {
	return m_id;
}

std::string InputPin::getType() const {
	return m_type;
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

ADERITE_EDITOR_NODE_NAMESPACE_END
