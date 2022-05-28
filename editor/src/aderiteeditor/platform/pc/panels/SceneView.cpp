#include "SceneView.hpp"

#include <bgfx/bgfx.h>
#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/PrefabAsset.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/shared/DragDrop.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/utility/ImGui.hpp"

namespace aderite {
namespace editor {

bool SceneView::init() {
    return true;
}

void SceneView::shutdown() {}

void SceneView::render() {
    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0, 0});
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    if (!ImGui::Begin("Scene view")) {
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    // Get current scene
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (currentScene == nullptr) {
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    // Prefab drag and drop
    utility::WindowSizeDragDrop([&]() {
        asset::PrefabAsset* prefabDrop = DragDrop::renderTarget<asset::PrefabAsset>(aderite::reflection::RuntimeTypes::PREFAB);
        if (prefabDrop != nullptr) {
            currentScene->createGameObject(prefabDrop);
        }
    });

    bgfx::TextureHandle outHandle = BGFX_INVALID_HANDLE;

    if (State::getInstance().IsGameMode) {
        // Show active camera output
        for (auto& gObject : currentScene->getGameObjects()) {
            scene::Camera* camera = gObject->getCamera();

            if (camera != nullptr) {
                outHandle = camera->getOutputHandle();
            }
        }
    } else {
        // Editor camera
        outHandle = editor::State::getInstance().getEditorCamera()->getOutputHandle();
    }

    if (!bgfx::isValid(outHandle)) {
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    ImVec2 viewportSize = {viewportPanelSize.x, viewportPanelSize.y};
    m_size.x = viewportPanelSize.x;
    m_size.y = viewportPanelSize.y;
    editor::State::getInstance().getEditorCamera()->onViewportResize(m_size);

    ImGui::Image((void*)(intptr_t)outHandle.idx, viewportSize, ImVec2(0, 0), ImVec2(1, 1));

    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace editor
} // namespace aderite
