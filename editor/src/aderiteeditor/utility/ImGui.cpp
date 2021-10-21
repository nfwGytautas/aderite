#include "ImGui.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace aderite {
namespace utility {

static int DynamicInputTextCallback(ImGuiInputTextCallbackData* data) {
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		std::string* str = (std::string*)data->UserData;
		//IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}
	return 0;
}

bool DynamicInputText(const char* label, std::string* value, ImGuiInputTextFlags flags) {
	return ImGui::InputText(label, (char*)value->c_str(), value->capacity() + 1, flags, DynamicInputTextCallback, value);
}

bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth) {
	bool altered = false;

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize)) {
		values.x = resetValue;
		altered = true;
	}
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f")) {
		altered = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize)) {
		values.y = resetValue;
		altered = true;
	}
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f")) {
		altered = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize)) {
		values.z = resetValue;
		altered = true;
	}
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f")) {
		altered = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

	ImGui::Dummy(ImVec2(0.0f, 2.5f));

	return altered;
}

bool InlineRename::renderUI() {
	static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
	bool renamed = false;

	if (!m_renaming) {
		if (ImGui::Selectable(m_initial.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				m_initial = "";
				m_renaming = true;
				m_appearing = true;
			}
		}
	}
	else {
		if (DynamicInputText("##rename", &m_value, edit_flags)) {
			m_renaming = false;

			if (m_value.empty()) {
				m_value = m_initial;
			}

			renamed = true;
		}

		if (!ImGui::IsItemActive() && !m_appearing) {
			m_renaming = false;
		}

		if (m_appearing) {
			ImGui::SetKeyboardFocusHere();
			m_appearing = false;
		}
	}

	return renamed;
}

bool InlineRename::renderUI(size_t index, std::string value) {
	static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
	bool renamed = false;

	if (m_idx != index) {
		if (ImGui::Selectable(value.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				value = "";
				m_renaming = true;
				m_appearing = true;
				m_idx = index;
			}
		}
	}
	else {
		if (DynamicInputText("##rename", &m_value, edit_flags)) {
			m_renaming = false;

			if (m_value.empty()) {
				m_value = value;
			}

			renamed = true;
			m_idx = -1;
		}

		if (!ImGui::IsItemActive() && !m_appearing) {
			m_renaming = false;
			m_idx = -1;
		}

		if (m_appearing) {
			ImGui::SetKeyboardFocusHere();
			m_appearing = false;
		}
	}

	return renamed;
}

void InlineRename::setState(bool state) {
	m_renaming = state;
}

void InlineRename::setValue(const std::string& value) {
	m_initial = value;
	m_value = value;
}

std::string InlineRename::getValue() const {
	return m_value;
}

}
}