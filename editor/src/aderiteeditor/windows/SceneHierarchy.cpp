#include "SceneHierarchy.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/selectors/TagSelector.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

#include "aderiteeditor/shared/Config.hpp"
#include "aderiteeditor/shared/DragDropObject.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/vfs/File.hpp"
#include "aderiteeditor/vfs/VFS.hpp"
#include "aderiteeditor/windows/Modals.hpp"
#include "aderiteeditor/windows/SelectScriptModal.hpp"

namespace aderite {
namespace editor_ui {

static ImGuiTreeNodeFlags c_BaseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                        ImGuiTreeNodeFlags_SpanAvailWidth;

SceneHierarchy::SceneHierarchy() {
    m_textModal = new TextInputModal();
}

SceneHierarchy::~SceneHierarchy() {
    delete m_textModal;
}

void SceneHierarchy::render() {
    static vfs::File* cacheFile = nullptr;

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
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Entities")) {
        this->renderEntities();
        ImGui::TreePop();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();

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

void SceneHierarchy::renderContextMenu() {
    static editor_ui::SelectScriptModal ssm;
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    // TODO: Move to their own tree contexts?

    if (ImGui::BeginPopupContextWindow()) {
        ImGui::MenuItem("(Entity)", NULL, false, false);

        if (ImGui::Selectable("Create Entity")) {
            // TODO: Make sure that this is actually unique
            scene::Entity* entity = new scene::Entity();
            currentScene->addEntity(entity);
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

        ImGui::MenuItem("(Scripts)", NULL, false, false);

        if (ImGui::BeginMenu("Create entity selector")) {
            if (ImGui::MenuItem("Tag selector")) {
                scene::EntitySelector* selector = new scene::TagSelector();
                selector->setName(utility::generateString(12));
                currentScene->addEntitySelector(selector);
            }

            ImGui::EndMenu();
        }

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

void SceneHierarchy::renderEntities() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    size_t idx = 0;
    for (scene::Entity* entity : currentScene->getEntities()) {
        ImGuiTreeNodeFlags leafFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Entity &&
            editor::State::LastSelectedObject.getSerializable() == entity) {
            leafFlags |= ImGuiTreeNodeFlags_Selected;
        }

        ImGui::TreeNodeEx(entity->getName().c_str(), leafFlags);

        // Context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                // TODO: Delete entity
            }

            ImGui::EndPopup();
        }

        // Drag drop
        if (ImGui::BeginDragDropSource()) {
            editor::DragDropObject obj {entity};
            ImGui::SetDragDropPayload(editor::DDPayloadID__Entity, &obj, sizeof(editor::DragDropObject));
            ImGui::EndDragDropSource();
        }

        // Selection
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            editor::State::LastSelectedObject = editor::SelectableObject(entity);
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

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Serializable &&
                editor::State::LastSelectedObject.getSerializable() == listener) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(listener->getName().c_str(), nodeFlags, "%s", listener->getName().c_str());

            if (ImGui::BeginDragDropSource()) {
                editor::DragDropObject obj {listener};
                ImGui::SetDragDropPayload(editor::DDPayloadID__AudioListener, &obj, sizeof(editor::DragDropObject));
                ImGui::EndDragDropSource();
            }

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

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Serializable &&
                editor::State::LastSelectedObject.getSerializable() == source) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(source->getName().c_str(), nodeFlags, "%s", source->getName().c_str());

            if (ImGui::BeginDragDropSource()) {
                editor::DragDropObject obj {source};
                ImGui::SetDragDropPayload(editor::DDPayloadID__AudioSource, &obj, sizeof(editor::DragDropObject));
                ImGui::EndDragDropSource();
            }

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
    if (ImGui::TreeNode("Entity selectors")) {
        size_t idx = 0;
        for (scene::EntitySelector* selector : currentScene->getEntitySelectors()) {
            ImGuiTreeNodeFlags nodeFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Serializable &&
                editor::State::LastSelectedObject.getSerializable() == selector) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(selector->getName().c_str(), nodeFlags, "%s", selector->getName().c_str());

            if (ImGui::BeginDragDropSource()) {
                editor::DragDropObject obj {selector};
                ImGui::SetDragDropPayload(editor::DDPayloadID__EntitySelector, &obj, sizeof(editor::DragDropObject));
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                editor::State::LastSelectedObject = editor::SelectableObject(selector);
            }
        }

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Systems")) {
        size_t idx = 0;
        for (scripting::ScriptSystem* system : currentScene->getScriptSystems()) {
            ImGuiTreeNodeFlags nodeFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Serializable &&
                editor::State::LastSelectedObject.getSerializable() == system) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(system->getName().c_str(), nodeFlags, "%s", system->getName().c_str());

            if (ImGui::BeginDragDropSource()) {
                editor::DragDropObject obj {system};
                ImGui::SetDragDropPayload(editor::DDPayloadID__ScriptSystem, &obj, sizeof(editor::DragDropObject));
                ImGui::EndDragDropSource();
            }

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

} // namespace editor_ui
} // namespace aderite
