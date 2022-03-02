#include "DragDropImportModal.hpp"

#include <imgui/imgui.h>

namespace aderite {
namespace editor {

DragDropImportModal::DragDropImportModal(std::filesystem::path path, ImportFn callback) : m_path(path), m_callback(callback) {}

void DragDropImportModal::render() {
    if (m_path.empty() || !m_callback) {
        this->close();
        return;
    }

    if (!m_open) {
        return;
    }

    // Center on screen
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(450.0f, 0.0f));
    if (ImGui::BeginPopupModal("Import asset", NULL,
                               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
        // Content
        ImGui::Text(("Importing " + m_path.string()).c_str());
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Text("Please select what type this file is:");

        // This will force buttons to be in the spaced line
        ImGui::Dummy(ImVec2(0, 0));

        // Asset type selection

        // Type radio button
        ImGui::RadioButton("Mesh", &m_type, static_cast<int>(reflection::RuntimeTypes::MESH));
        ImGui::RadioButton("Texture", &m_type, static_cast<int>(reflection::RuntimeTypes::TEXTURE));

        // Import button and cancel
        const float buttonWidth = 75.0f;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;

        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        ImGui::SameLine(ImGui::GetWindowWidth() - (buttonWidth + spacing));
        if (ImGui::Button("Import", ImVec2(buttonWidth, 0.0f))) {
            // Check type
            if (m_type != -1) {
                // Try to import the asset
                this->m_callback(m_path, static_cast<reflection::RuntimeTypes>(m_type));
            }

            this->close();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - (buttonWidth + spacing) * 2);
        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
            // Cancel pressed, so just ignore this
            this->close();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

bool DragDropImportModal::stillValid() const {
    return m_open;
}

void DragDropImportModal::close() {
    m_open = false;
}

void DragDropImportModal::show() {
    ImGui::OpenPopup("Import asset");
}

} // namespace editor
} // namespace aderite
