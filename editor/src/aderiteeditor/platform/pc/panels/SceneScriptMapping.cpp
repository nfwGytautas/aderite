#include "SceneScriptMapping.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/events/ScriptUpdateEvent.hpp"

#include "aderiteeditor/platform/pc/WindowsEditor.hpp"
#include "aderiteeditor/platform/pc/modals/SelectScriptModal.hpp"

namespace aderite {
namespace editor {

SceneScriptMapping::SceneScriptMapping() {}

SceneScriptMapping::~SceneScriptMapping() {}

bool SceneScriptMapping::init() {
    return true;
}

void SceneScriptMapping::shutdown() {}

void SceneScriptMapping::render() {
    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    // Get current scene
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (!ImGui::Begin("Script mapping") || currentScene == nullptr) {
        ImGui::End();
        return;
    }

    scripting::ScriptEventMap* eventMap = currentScene;

    if (ImGui::CollapsingHeader("Update", ImGuiTreeNodeFlags_None)) {

        ImGui::PushItemWidth(-FLT_MIN);
        for (scripting::ScriptUpdateEvent* sue : eventMap->getUpdateEvents()) {
            ImGui::Selectable(sue->getFullName().c_str());
        }
        ImGui::PopItemWidth();

        float width = ImGui::GetContentRegionAvail().x * 0.4855f;
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
        if (ImGui::Button("Add", ImVec2(width, 0.0f))) {
            WindowsEditor::getInstance()->getUI().pushModal(new SelectScriptModal(
                SelectScriptModal::FilterType::UPDATE, std::bind(&SceneScriptMapping::addUpdateEvent, this, std::placeholders::_1)));
        }
    }

    ImGui::End();
}

void SceneScriptMapping::addUpdateEvent(scripting::ScriptEvent* e) {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
    scripting::ScriptEventMap* eventMap = currentScene;
    eventMap->add(static_cast<scripting::ScriptUpdateEvent*>(e));
}

} // namespace editor
} // namespace aderite
