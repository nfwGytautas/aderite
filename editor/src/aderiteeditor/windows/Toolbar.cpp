#include "Toolbar.hpp"
#include <filesystem>
#include <vector>

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor_ui {

Toolbar::Toolbar() {}

Toolbar::~Toolbar() {}

void Toolbar::render() {
    if (!ImGui::Begin("Toolbar")) {
        ImGui::End();
        return;
    }

    if (ImGui::Selectable("Start", editor::State::IsGameMode, editor::State::IsGameMode ? ImGuiSelectableFlags_Disabled : 0,
                          ImVec2(50, 0))) {
        editor::State::Sink->onStartGame();
    }

    ImGui::SameLine();

    if (ImGui::Selectable("Stop", !editor::State::IsGameMode, editor::State::IsGameMode ? 0 : ImGuiSelectableFlags_Disabled,
                          ImVec2(50, 0))) {
        editor::State::Sink->onStopGame();
    }

    ImGui::End();
}

} // namespace editor_ui
} // namespace aderite
