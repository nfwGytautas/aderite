#include "SelectScriptModal.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptClass.hpp"
#include "aderite/scripting/ScriptEvent.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/events/ScriptUpdateEvent.hpp"

#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/utility/ImGui.hpp"

namespace aderite {
namespace editor {

SelectScriptModal::SelectScriptModal(FilterType filter, SelectFn fn) : m_filtering(filter), m_callback(fn) {}

void SelectScriptModal::render() {
    if (!m_callback) {
        this->close();
    }

    if (!m_open) {
        return;
    }

    // Center on screen
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Select script", NULL,
                               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushItemWidth(-FLT_MIN);

        if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0.0f))) {
            this->close();
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopItemWidth();

        ImGui::Dummy(ImVec2(500.0f, 0.0f));
        ImGui::Separator();

        if (m_class == nullptr) {
            // Select class
            if (ImGui::BeginListBox("##ScriptSelectClass", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
                for (scripting::ScriptClass* sc : ::aderite::Engine::getScriptManager()->getScripts()) {
                    if (ImGui::Selectable(sc->getName())) {
                        m_class = sc;
                    }
                }

                ImGui::EndListBox();
            }
        } else {
            // Select method
            if (ImGui::BeginListBox("##ScriptSelectMethod", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
                switch (m_filtering) {
                case FilterType::UPDATE: {
                    for (scripting::ScriptUpdateEvent* sue : m_class->getUpdateEvents()) {
                        if (ImGui::Selectable(sue->getName())) {
                            m_event = sue;
                        }
                    }
                }
                }

                if (m_event != nullptr) {
                    m_callback(m_event);
                    this->close();
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndListBox();
            }
        }

        ImGui::EndPopup();
    }
}

bool SelectScriptModal::stillValid() const {
    return m_open;
}

void SelectScriptModal::close() {
    m_open = false;
}

void SelectScriptModal::show() {
    ImGui::OpenPopup("Select script");
}

} // namespace editor
} // namespace aderite
