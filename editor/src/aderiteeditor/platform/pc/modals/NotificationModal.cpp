#include "NotificationModal.hpp"

#include <imgui/imgui.h>

namespace aderite {
namespace editor {

NotificationModal::NotificationModal(const std::string& header, const std::string& message) : m_header(header), m_message(message) {}

void NotificationModal::render() {
    if (!m_open) {
        return;
    }

    // Center on screen
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(450.0f, 0.0f));
    if (ImGui::BeginPopupModal(m_header.c_str(), NULL,
                               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
        // Content
        ImGui::Text(m_message.c_str());
        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        // This will force buttons to be in the spaced line
        ImGui::Dummy(ImVec2(0, 0));

        // Close button
        const float buttonWidth = 75.0f;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;

        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        ImGui::SameLine(ImGui::GetWindowWidth() - (buttonWidth + spacing));
        if (ImGui::Button("Close", ImVec2(buttonWidth, 0.0f))) {
            this->close();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

bool NotificationModal::stillValid() const {
    return m_open;
}

void NotificationModal::close() {
    m_open = false;
}

void NotificationModal::show() {
    ImGui::OpenPopup(m_header.c_str());
}

} // namespace editor
} // namespace aderite
