#include "SelectAudioModal.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioController.hpp"

#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/utility/ImGui.hpp"

namespace aderite {
namespace editor {

void SelectAudioModal::show() {
    m_visible = true;
    m_show = true;
}

bool SelectAudioModal::isOpen() const {
    return m_visible;
}

std::string SelectAudioModal::getSelectedEvent() const {
    return m_selected;
}

void SelectAudioModal::reset() {
    m_selected = "";
}

bool SelectAudioModal::init() {
    return true;
}

void SelectAudioModal::shutdown() {}

void SelectAudioModal::render() {
    if (m_show) {
        ImGui::OpenPopup("Select event");
        m_show = false;
    }

    // Center on screen
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Select event", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0.0f))) {
            m_visible = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopItemWidth();

        ImGui::Dummy(ImVec2(500.0f, 0.0f));
        ImGui::Separator();

        // TODO: Filtering

        if (ImGui::BeginListBox("##AudioSelectList", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
            for (std::string eventName : ::aderite::Engine::getAudioController()->getKnownEvents()) {
                if (ImGui::Selectable(eventName.c_str())) {
                    m_selected = eventName;
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

}
}
