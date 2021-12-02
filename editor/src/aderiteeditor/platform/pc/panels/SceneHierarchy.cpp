#include "SceneHierarchy.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/Scenery.hpp"
#include "aderite/scene/Visual.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

#include "aderiteeditor/platform/pc/modals/SelectScriptModal.hpp"
#include "aderiteeditor/shared/DragDrop.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/vfs/File.hpp"
#include "aderiteeditor/vfs/VFS.hpp"

namespace aderite {
namespace editor {

static ImGuiTreeNodeFlags c_BaseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                        ImGuiTreeNodeFlags_SpanAvailWidth;

SceneHierarchy::SceneHierarchy() {}

SceneHierarchy::~SceneHierarchy() {}

void SceneHierarchy::renderContextMenu() {
    static SelectScriptModal ssm;
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    // TODO: Move to their own tree contexts?

    if (ImGui::BeginPopupContextWindow()) {
        ImGui::MenuItem("(Objects)", NULL, false, false);

        if (ImGui::MenuItem("Add visual")) {
            currentScene->addVisual(new scene::Visual());
        }

        if (ImGui::MenuItem("Add scenery")) {
        }

        if (ImGui::MenuItem("Add object")) {
        }

        ImGui::Separator();

        ImGui::MenuItem("(Audio)", NULL, false, false);

        if (ImGui::MenuItem("Create source")) {
            audio::AudioSource* source = new audio::AudioSource();
            currentScene->addAudioSource(source);
        }

        if (ImGui::MenuItem("Create listener")) {
            audio::AudioListener* listener = new audio::AudioListener();
            currentScene->addAudioListener(listener);
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Create system")) {
            ssm.show();
        }

        ImGui::Separator();

        ImGui::MenuItem("(Scene)", NULL, false, false);

        if (ImGui::Selectable("Create tag", false, currentScene->getFreeTagSlots() == 0 ? ImGuiSelectableFlags_Disabled : 0)) {
            currentScene->addTag(utility::generateString(12));
        }

        if (ImGui::Selectable("Edit")) {
            editor::State::LastSelectedObject = editor::SelectableObject(currentScene);
        }

        ImGui::EndPopup();
    }

    ssm.render();

    if (!ssm.getSelected().empty()) {
        scripting::ScriptSystem* system = new scripting::ScriptSystem();
        system->load(ssm.getSelected());
        currentScene->addScriptSystem(system);
        ssm.reset();
    }
}

void SceneHierarchy::renderVisuals() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    size_t idx = 0;
    for (scene::Visual* visual : currentScene->getVisuals()) {
        ImGuiTreeNodeFlags leafFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Entity &&
            editor::State::LastSelectedObject.getSerializable() == visual) {
            leafFlags |= ImGuiTreeNodeFlags_Selected;
        }

        ImGui::TreeNodeEx(visual->getName().c_str(), leafFlags);

        // Context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                currentScene->removeVisual(visual);

                // Quit and rerender
                ImGui::EndPopup();
                break;
            }

            ImGui::EndPopup();
        }

        // Drag drop
        DragDrop::renderSource(visual);

        // Selection
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            editor::State::LastSelectedObject = editor::SelectableObject(visual);
        }
    }
}

void SceneHierarchy::renderScenery() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    size_t idx = 0;
    for (scene::Scenery* scenery : currentScene->getScenery()) {
        ImGuiTreeNodeFlags leafFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Entity &&
            editor::State::LastSelectedObject.getSerializable() == scenery) {
            leafFlags |= ImGuiTreeNodeFlags_Selected;
        }

        ImGui::TreeNodeEx(scenery->getName().c_str(), leafFlags);

        // Context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                currentScene->removeScenery(scenery);

                // Quit and rerender
                ImGui::EndPopup();
                break;
            }

            ImGui::EndPopup();
        }

        // Drag drop
        DragDrop::renderSource(scenery);

        // Selection
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            editor::State::LastSelectedObject = editor::SelectableObject(scenery);
        }
    }
}

void SceneHierarchy::renderAudio() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Listeners")) {
        size_t idx = 0;
        for (audio::AudioListener* listener : currentScene->getAudioListeners()) {
            ImGuiTreeNodeFlags nodeFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::NamedSerializable &&
                editor::State::LastSelectedObject.getSerializable() == listener) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(listener->getName().c_str(), nodeFlags, "%s", listener->getName().c_str());

            DragDrop::renderSource(listener);

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                editor::State::LastSelectedObject = editor::SelectableObject(listener);
            }
        }

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Sources")) {
        size_t idx = 0;
        for (audio::AudioSource* source : currentScene->getAudioSources()) {
            ImGuiTreeNodeFlags nodeFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::NamedSerializable &&
                editor::State::LastSelectedObject.getSerializable() == source) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(source->getName().c_str(), nodeFlags, "%s", source->getName().c_str());

            DragDrop::renderSource(source);

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                editor::State::LastSelectedObject = editor::SelectableObject(source);
            }
        }

        ImGui::TreePop();
    }
}

void SceneHierarchy::renderScripts() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Systems")) {
        size_t idx = 0;
        for (scripting::ScriptSystem* system : currentScene->getScriptSystems()) {
            ImGuiTreeNodeFlags nodeFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::NamedSerializable &&
                editor::State::LastSelectedObject.getSerializable() == system) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(system->getName().c_str(), nodeFlags, "%s", system->getName().c_str());

            DragDrop::renderSource(system);

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                editor::State::LastSelectedObject = editor::SelectableObject(system);
            }
        }

        ImGui::TreePop();
    }
}

void SceneHierarchy::renderSettings() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Tags")) {
        for (const std::string& tag : currentScene->getTags()) {
            if (!tag.empty()) {
                ImGuiTreeNodeFlags nodeFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                ImGui::TreeNodeEx(tag.c_str(), nodeFlags, "%s", tag.c_str());
                // TODO: Edit tag name
            }
        }

        ImGui::TreePop();
    }
}

bool SceneHierarchy::init() {
    return true;
}

void SceneHierarchy::shutdown() {}

void SceneHierarchy::render() {
    static vfs::File* cacheFile = nullptr;

    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    if (!ImGui::Begin("Scene hierarchy")) {
        ImGui::End();
        return;
    }

    // Root node name is the scene name
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (!currentScene) {
        ImGui::End();
        return;
    }

    if (cacheFile == nullptr || cacheFile->getHandle() != currentScene->getHandle()) {
        cacheFile = editor::State::Project->getVfs()->getFile(currentScene->getHandle());
    }

    // Context menu
    this->renderContextMenu();

    ImGui::Text("%s", cacheFile->getName().c_str());

    // Actual tree
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Visuals")) {
        this->renderVisuals();
        ImGui::TreePop();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Scenery")) {
        this->renderScenery();
        ImGui::TreePop();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();

    /*ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Entities")) {
        asset::PrefabAsset* prefab = DragDrop::renderTarget<asset::PrefabAsset>(reflection::RuntimeTypes::PREFAB);
        if (prefab != nullptr) {
            if (prefab->getPrototype() != nullptr) {
                prefab->instantiate(currentScene);
            }
        }

        this->renderEntities();
        ImGui::TreePop();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();*/

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Audio")) {
        this->renderAudio();
        ImGui::TreePop();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Scripts")) {
        this->renderScripts();
        ImGui::TreePop();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Settings")) {
        this->renderSettings();
        ImGui::TreePop();
    }

    ImGui::End();
}

} // namespace editor
} // namespace aderite
