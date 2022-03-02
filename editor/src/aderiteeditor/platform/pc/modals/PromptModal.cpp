#include "PromptModal.hpp"

#include <imgui/imgui.h>

#include "aderite/utility/Log.hpp"

namespace aderite {
namespace editor {

PromptModal::PromptModal(const std::string& prompt, const std::string& content) : m_prompt(prompt), m_content(content) {}

void PromptModal::pushButton(PromptButton button) {
    m_buttons.push_back(button);
}

void PromptModal::render() {
    if (m_buttons.size() == 0) {
        LOG_WARN("[Editor] Prompt with no buttons rendered, aborting this modal");
        m_open = false;
    }

    if (!m_open) {
        return;
    }

    // Center on screen
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(450.0f, 0.0f));
    if (ImGui::BeginPopupModal(m_prompt.c_str(), NULL,
                               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
        // Content
        ImGui::TextWrapped(m_content.c_str());
        ImGui::Dummy(ImVec2(0.0f, 15.0f));

        // This will force buttons to be in the spaced line
        ImGui::Dummy(ImVec2(0, 0));

        // Buttons
        const float buttonWidth = 75.0f;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        float offset = buttonWidth + spacing;
        for (PromptButton& btn : m_buttons) {
            ImGui::SameLine(ImGui::GetWindowWidth() - offset);

            if (ImGui::Button(btn.Label, ImVec2(buttonWidth, 0.0f))) {
                if (btn.Callback) {
                    btn.Callback();
                }

                m_open = false;
                ImGui::CloseCurrentPopup();
            }

            offset += buttonWidth + spacing;
        }

        ImGui::EndPopup();
    }
}

bool PromptModal::stillValid() const {
    return m_open;
}

void PromptModal::close() {
    m_open = false;
}

void PromptModal::show() {
    ImGui::OpenPopup(m_prompt.c_str());
}

} // namespace editor
} // namespace aderite
