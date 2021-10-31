#include "SelectScriptModal.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/scripting/BehaviorWrapper.hpp"
#include "aderite/scripting/ScriptManager.hpp"

#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/utility/ImGui.hpp"
#include "aderiteeditor/windows/FileDialog.hpp"
#include "aderiteeditor/windows/Modals.hpp"
#include "aderiteeditor/windows/StartupModal.hpp"

namespace aderite {
namespace editor_ui {

void SelectScriptModal::show() {
    m_visible = true;
    m_show = true;
}

void SelectScriptModal::render() {
    if (m_show) {
        ImGui::OpenPopup("Select script");
        m_show = false;
    }

    // Center on screen
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Select script", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0.0f))) {
            m_visible = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopItemWidth();

        ImGui::Dummy(ImVec2(500.0f, 0.0f));
        ImGui::Separator();

        // TODO: Filtering

        if (ImGui::BeginListBox("##ScriptSelectList", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (scripting::BehaviorWrapper* bw : ::aderite::Engine::getScriptManager()->getAllBehaviors()) {
                if (ImGui::Selectable(bw->getName().c_str())) {
                    m_selected = bw;
                    m_visible = false;
                    ImGui::CloseCurrentPopup();
                    break;
                }
            }

            ImGui::EndListBox();
        }

        ImGui::EndPopup();
    }
}

bool SelectScriptModal::isOpen() const {
    return m_visible;
}

scripting::BehaviorWrapper* SelectScriptModal::getSelectedBehavior() const {
    return m_selected;
}

void SelectScriptModal::reset() {
    m_selected = nullptr;
}

} // namespace editor_ui
} // namespace aderite
