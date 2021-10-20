#include "Toolbar.hpp"

#include <vector>
#include <filesystem>

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

Toolbar::Toolbar() {
}

Toolbar::~Toolbar() {
}

void Toolbar::render() {
	if (!ImGui::Begin("Toolbar")) {
		ImGui::End();
		return;
	}

	if (ImGui::Selectable("Start", editor::State::IsGameMode, editor::State::IsGameMode ? ImGuiSelectableFlags_Disabled : 0, ImVec2(50, 0))) {
		editor::State::Sink->onStartGame();
	}

	ImGui::SameLine();

	if (ImGui::Selectable("Stop", !editor::State::IsGameMode, editor::State::IsGameMode ? 0 : ImGuiSelectableFlags_Disabled, ImVec2(50, 0))) {
		editor::State::Sink->onStopGame();
	}

	ImGui::End();
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
