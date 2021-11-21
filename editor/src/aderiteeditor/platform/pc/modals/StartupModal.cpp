#include "StartupModal.hpp"

#include <imgui/imgui.h>

#include "aderiteeditor/platform/pc/modals/FileDialog.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/utility/ImGui.hpp"

namespace aderite {
namespace editor {

void StartupModal::show() {
    m_visible = true;
    m_show = true;
}

bool StartupModal::isOpen() const {
    return m_visible;
}

void StartupModal::renderMain() {
    ImGui::Text("Welcome to the aderite editor");
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Text("Load existing project or create a new one");
    ImGui::Separator();

    float width = ImGui::GetContentRegionAvail().x;

    if (ImGui::Button("New project", ImVec2(width, 0.0f))) {
        m_newProjectName = "";
        m_newProjectRoot = "";
        m_newProjectRootPreview = "";
        m_newProject = true;
    }

    ImGui::Spacing();

    if (ImGui::Button("Load project", ImVec2(width, 0.0f))) {
        std::string file = editor::FileDialog::selectFile("Select aderite project", {"Aderite project", "*.aproj"});

        if (!file.empty()) {
            editor::State::Sink->onLoadProject(file);
            m_visible = false;
            ImGui::CloseCurrentPopup();
        }
    }

    // TODO: Recent projects
}

void StartupModal::renderNewProject() {
    static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank |
                                            ImGuiInputTextFlags_EnterReturnsTrue;

    float modalWidth = ImGui::GetContentRegionAvail().x;

    ImGui::Text("New project");
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();

    if (ImGui::BeginTable("NewProjectTable", 2, ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Project name");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        utility::DynamicInputText("###projectName", &m_newProjectName, edit_flags);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Project root");
        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button(m_newProjectRootPreview.empty() ? "Select..." : m_newProjectRootPreview.c_str(), ImVec2(-1, 0.0f))) {
            std::string dir = editor::FileDialog::selectDirectory();
            m_newProjectRoot = dir;

            if (m_newProjectRoot.size() > 15) {
                m_newProjectRootPreview = "..." + m_newProjectRoot.substr(m_newProjectRoot.size() - 15);
            } else {
                m_newProjectRootPreview = m_newProjectRoot;
            }
        }

        ImGui::EndTable();
    }

    // TODO: Templates
    ImGui::Text("Template:");
    ImGui::RadioButton("None", &m_template, 0);

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    float half = modalWidth * 0.4875;
    if (ImGui::Button("Back", ImVec2(half, 0.0f))) {
        m_newProject = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Create", ImVec2(half, 0.0f))) {
        // Validate
        if (m_newProjectName.size() > 0 && m_newProjectRoot.size() > 0) {
            // Create project
            editor::State::Sink->onNewProject(m_newProjectRoot, m_newProjectName);
            m_visible = false;
            ImGui::CloseCurrentPopup();
        }
    }
}

bool aderite::editor::StartupModal::init() {
    this->show();
    return true;
}

void aderite::editor::StartupModal::shutdown() {}

void StartupModal::render() {
    if (this->isProjectLoaded()) {
        // A project exists don't render startup modal

        // TODO: Remove when done
        // DEMO WINDOW
        static bool show_demo_window = true;
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        return;
    }

    if (m_show) {
        ImGui::OpenPopup("Startup");
        m_show = false;
    }

    // Center on screen
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Startup", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        if (!m_newProject) {
            renderMain();
        } else {
            renderNewProject();
        }

        ImGui::EndPopup();
    }
}

} // namespace editor
} // namespace aderite
