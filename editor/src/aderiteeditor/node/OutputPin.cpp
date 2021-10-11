#include "OutputPin.hpp"

#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

OutputPin::OutputPin(int id, const std::string& type, const std::string& name)
	: m_id(id), m_type(type), m_name(name)
{}

int OutputPin::getId() const {
	return m_id;
}

std::string OutputPin::getType() const {
	return m_type;
}

std::string OutputPin::getName() const {
	return m_name;
}

void OutputPin::renderUI() {
	ImNodes::BeginOutputAttribute(m_id);
	ImGui::Text(m_name.c_str());
	ImGui::SameLine();
	ImGui::Text(m_type.c_str());
	ImNodes::EndOutputAttribute();
}

ADERITE_EDITOR_NODE_NAMESPACE_END
