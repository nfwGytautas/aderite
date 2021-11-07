#include "SceneHierarchy.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

#include "aderiteeditor/extensions/EditorAudioSource.hpp"
#include "aderiteeditor/extensions/EditorEntity.hpp"
#include "aderiteeditor/shared/Config.hpp"
#include "aderiteeditor/shared/DragDropObject.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/vfs/File.hpp"
#include "aderiteeditor/vfs/VFS.hpp"
#include "aderiteeditor/windows/Modals.hpp"

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
    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
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
    if (ImGui::TreeNode("Physics")) {
        this->renderPhysics();
        ImGui::TreePop();
    }

    ImGui::End();
}

void SceneHierarchy::renderContextMenu() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    // TODO: Move to their own tree contexts?

    if (ImGui::BeginPopupContextWindow()) {
        ImGui::MenuItem("(Entity)", NULL, false, false);

        if (ImGui::Selectable("Create Entity")) {
            // TODO: Make sure that this is actually unique
            scene::EditorEntity* entity = new scene::EditorEntity();
            currentScene->addEntity(entity);
        }

        ImGui::Separator();

        ImGui::MenuItem("(Audio)", NULL, false, false);

        if (ImGui::MenuItem("Create source")) {
            audio::EditorAudioSource* source = new audio::EditorAudioSource();
            currentScene->addSource(source);
            ::aderite::Engine::getAudioController()->addSource(source);
        }

        ImGui::Separator();

        ImGui::MenuItem("(Scene)", NULL, false, false);

        if (ImGui::Selectable("Edit")) {
            editor::State::LastSelectedObject = editor::SelectableObject(currentScene);
        }

        ImGui::EndPopup();
    }
}

void SceneHierarchy::renderEntities() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    size_t idx = 0;
    for (scene::Entity* entity : currentScene->getEntities()) {
        scene::EditorEntity* editorEntity = static_cast<scene::EditorEntity*>(entity);

        ImGuiTreeNodeFlags leafFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Entity &&
            editor::State::LastSelectedObject.getSerializable() == entity) {
            leafFlags |= ImGuiTreeNodeFlags_Selected;
        }

        ImGui::TreeNodeEx(editorEntity->getName().c_str(), leafFlags);

        // Context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                // TODO: Delete entity   
            }

            ImGui::EndPopup();
        }

        // Drag drop
        if (ImGui::BeginDragDropSource()) {
            editor::DragDropObject obj {editorEntity};
            ImGui::SetDragDropPayload(editor::DDPayloadID__Entity, &obj, sizeof(editor::DragDropObject));
            ImGui::EndDragDropSource();
        }

        // Selection
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            editor::State::LastSelectedObject = editor::SelectableObject(editorEntity);
        }
    }
}

void SceneHierarchy::renderAudio() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Sources")) {
        size_t idx = 0;
        for (audio::AudioSource* source : currentScene->getAudioSources()) {
            audio::EditorAudioSource* editorSource = static_cast<audio::EditorAudioSource*>(source);

            ImGuiTreeNodeFlags nodeFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Serializable &&
                editor::State::LastSelectedObject.getSerializable() == editorSource) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx(editorSource->getName().c_str(), nodeFlags, "%s", editorSource->getName().c_str());

            if (ImGui::BeginDragDropSource()) {
                editor::DragDropObject obj {editorSource};
                ImGui::SetDragDropPayload(editor::DDPayloadID__AudioSource, &obj, sizeof(editor::DragDropObject));
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                editor::State::LastSelectedObject = editor::SelectableObject(editorSource);
            }
        }

        ImGui::TreePop();
    }
}

void SceneHierarchy::renderPhysics() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Dynamic actors")) {
        /*size_t idx = 0;
        for (audio::AudioSource* source : currentScene->getAudioSources()) {
            audio::EditorAudioSource* editorSource = static_cast<audio::EditorAudioSource*>(source);

            ImGuiTreeNodeFlags nodeFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Serializable &&
                editor::State::LastSelectedObject.getSerializable() == editorSource) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx((void*)&idx, nodeFlags, "%s", editorSource->getName().c_str());

            if (ImGui::BeginDragDropSource()) {
                editor::DragDropObject obj {editorSource};
                ImGui::SetDragDropPayload(editor::DDPayloadID__AudioSource, &obj, sizeof(editor::DragDropObject));
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                editor::State::LastSelectedObject = editor::SelectableObject(editorSource);
            }
        }*/

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Static actors")) {
        /*size_t idx = 0;
        for (audio::AudioSource* source : currentScene->getAudioSources()) {
            audio::EditorAudioSource* editorSource = static_cast<audio::EditorAudioSource*>(source);

            ImGuiTreeNodeFlags nodeFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Serializable &&
                editor::State::LastSelectedObject.getSerializable() == editorSource) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx((void*)&idx, nodeFlags, "%s", editorSource->getName().c_str());

            if (ImGui::BeginDragDropSource()) {
                editor::DragDropObject obj {editorSource};
                ImGui::SetDragDropPayload(editor::DDPayloadID__AudioSource, &obj, sizeof(editor::DragDropObject));
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                editor::State::LastSelectedObject = editor::SelectableObject(editorSource);
            }
        }*/

        ImGui::TreePop();
    }
}

} // namespace editor_ui
} // namespace aderite
