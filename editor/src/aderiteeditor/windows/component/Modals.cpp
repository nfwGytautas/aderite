#include "modals.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderiteeditor/windows/component/ComponentUtility.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

constexpr size_t input_length = 64;

TextInputModal::TextInputModal(const std::string& title, const std::string& text, const Options& options)
	: m_text(text), m_title(title), m_options(options)
{
	m_text.resize(input_length, ' ');
}

void TextInputModal::show() {
	m_show = true;
}

void TextInputModal::setText(const std::string& text) {
	m_text = text;
}

void TextInputModal::setTitle(const std::string& text) {
	m_title = text;
}

void TextInputModal::setConfirmAction(ConfirmFn fn) {
	m_confirm = std::move(fn);
}

void TextInputModal::setCancelAction(CancelFn fn) {
	m_cancel = std::move(fn);
}

void TextInputModal::setConfirmButtonText(const std::string& text) {
	m_confirmText = text;
}

void TextInputModal::render() {
	if (m_show) {
		ImGui::OpenPopup(m_title.c_str());

		// Reset 
		m_value = "";
		m_show = false;
	}

	// Center on screen
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	// Input flags
	ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_CallbackResize;
	if (!m_options.AllowEmpty) {
		input_flags |= ImGuiInputTextFlags_CharsNoBlank;
	}

	if (ImGui::BeginPopupModal(m_title.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%s", m_text.c_str());
		ImGui::SameLine();
		DynamicInputText("", &m_value, input_flags);
		ImGui::Separator();

		float width = ImGui::GetContentRegionAvail().x * 0.4855f;
		if (ImGui::Button(m_confirmText.c_str(), ImVec2(width, 0.0f))) {
			if (m_confirm) {
				bool valid = true;

				if (!m_options.AllowEmpty) {
					if (m_value.empty() || m_value.find_first_not_of(' ') == std::string::npos) {
						valid = false;
					}
				}

				if (valid) {
					m_confirm(m_value);
				}
				else {
					// Cancel if not valid
					// TODO: Error text?
					if (m_cancel) {
						m_cancel();
					}
				}
			}

			ImGui::CloseCurrentPopup(); 
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		
		if (ImGui::Button("Cancel", ImVec2(width, 0.0f))) {
			if (m_cancel) {
				m_cancel();
			}

			ImGui::CloseCurrentPopup(); 
		}

		ImGui::EndPopup();
	}
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
