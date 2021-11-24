#include "Toolbar.hpp"
#include <filesystem>
#include <vector>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor {

Toolbar::Toolbar() {}

Toolbar::~Toolbar() {}

bool Toolbar::init() {
    return true;
}

void Toolbar::shutdown() {}

void Toolbar::render() {
    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse;

    //ImGui::SetNextWindowSize(ImVec2(ImGui::GetWindowWidth(), 25));

    if (!ImGui::Begin("Toolbar", nullptr, flags)) {
        ImGui::End();
        return;
    }

    ImGuiDockNode* node = ImGui::GetWindowDockNode();
    node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoDockingSplitMe |
                        ImGuiDockNodeFlags_NoDockingOverMe;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

    this->setCursorToCenter(24, 40);
    if (editor::State::IsGameMode) {
        if (this->renderIconButton("stop", 24, 24)) {
            editor::State::Sink->onStopGame();
        }
    } else {
        if (this->renderIconButton("play", 24, 24)) {
            editor::State::Sink->onStartGame();
        }
    }

    ImGui::PopStyleVar();

    ImGui::End();
}

} // namespace editor
} // namespace aderite
