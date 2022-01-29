#include "SceneHierarchy.hpp"
#include <functional>

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/PrefabAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

#include "aderiteeditor/platform/pc/modals/SelectScriptModal.hpp"
#include "aderiteeditor/shared/DragDrop.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/utility/ImGui.hpp"

namespace aderite {
namespace editor {

static ImGuiTreeNodeFlags c_BaseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                        ImGuiTreeNodeFlags_SpanAvailWidth;

SceneHierarchy::SceneHierarchy() {}

SceneHierarchy::~SceneHierarchy() {}

bool SceneHierarchy::init() {
    return true;
}

void SceneHierarchy::shutdown() {}

void SceneHierarchy::render() {
    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    // Get current scene
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (!ImGui::Begin("Scene hierarchy") || currentScene == nullptr) {
        ImGui::End();
        return;
    }

    // Context menu
    this->contextMenu();

    // Prefab drag and drop
    utility::WindowSizeDragDrop([&]() {
        asset::PrefabAsset* prefabDrop = DragDrop::renderTarget<asset::PrefabAsset>(aderite::reflection::RuntimeTypes::PREFAB);
        if (prefabDrop != nullptr) {
            currentScene->createGameObject(prefabDrop);
        }
    });

    ImGui::Text("%s", currentScene->getName().c_str());

    // Actual tree
    ImGui::Separator();

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    for (const auto& gameObject : currentScene->getGameObjects()) {
        ImGuiTreeNodeFlags leafFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        io::SerializableObject* selectedObject = editor::State::getInstance().getSelectedObject();
        if (selectedObject != nullptr && selectedObject == gameObject.get()) {
            leafFlags |= ImGuiTreeNodeFlags_Selected;
        }

        ImGui::TreeNodeEx(gameObject->getName().c_str(), leafFlags);

        // Context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                if (selectedObject != nullptr && selectedObject == gameObject.get()) {
                    editor::State::getInstance().setSelectedObject(nullptr);
                }

                currentScene->destroyGameObject(gameObject.get());

                // Quit and rerender
                ImGui::EndPopup();
                break;
            }

            ImGui::EndPopup();
        }

        // Drag drop
        DragDrop::renderSource(gameObject.get());

        // Selection
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            editor::State::getInstance().setSelectedObject(gameObject.get());
        }
    }

    ImGui::End();
}

void SceneHierarchy::contextMenu() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::BeginMenu("New object")) {
            if (ImGui::MenuItem("Empty")) {
                currentScene->createGameObject();
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();
        ImGui::MenuItem("(Scene)", NULL, false, false);

        if (ImGui::Selectable("Edit")) {
            editor::State::getInstance().setSelectedObject(currentScene);
        }

        ImGui::EndPopup();
    }
}

} // namespace editor
} // namespace aderite
