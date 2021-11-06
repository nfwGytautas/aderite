#include "SceneHierarchy.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

#include "aderiteeditor/extensions/EditorAudioSource.hpp"
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

    ImGui::End();
}

void SceneHierarchy::renderContextMenu() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::Selectable("Create Entity")) {
            // TODO: Make sure that this is actually unique
            currentScene->createEntity(::aderite::scene::MetaComponent(utility::generateString(16)));
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("Audio")) {
            if (ImGui::MenuItem("Create source")) {
                audio::EditorAudioSource* source = new audio::EditorAudioSource();
                currentScene->addSource(source);
                ::aderite::Engine::getAudioController()->addSource(source);
            }

            ImGui::EndMenu();
        }

        if (ImGui::Selectable("Edit")) {
            editor::State::LastSelectedObject = editor::SelectableObject(currentScene);
        }

        ImGui::EndPopup();
    }
}

void SceneHierarchy::renderEntities() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 2));
    for (auto [Entity, MetaComponent] : currentScene->getEntityRegistry().view<::aderite::scene::MetaComponent>().each()) {
        scene::Entity e = scene::Entity(Entity, currentScene);

        // Tree node
        bool has_children = false;

        if (has_children) {
            // If this is a Entity with children
            // TODO: TreeNodeEx (ImGui Selectable Node example)
        } else {
            // If this is a single Entity
            ImGuiTreeNodeFlags node_flags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Entity &&
                editor::State::LastSelectedObject.getEntity() == Entity) {
                node_flags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx((void*)(intptr_t)(uint32_t)Entity, node_flags, "%s", MetaComponent.Name.c_str());

            if (ImGui::IsItemClicked()) {
                editor::State::LastSelectedObject = editor::SelectableObject(e);
            }
        }

        // Context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::Selectable("Delete")) {
                currentScene->destroyEntity(e);
            }

            ImGui::EndPopup();
        }
    }
    ImGui::PopStyleVar();
}

void SceneHierarchy::renderAudio() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Sources")) {
        size_t idx = 0;
        for (audio::AudioSource* source : currentScene->getAudioSources()) {
            audio::EditorAudioSource* editorSource = static_cast<audio::EditorAudioSource*>(source);

            ImGuiTreeNodeFlags nodeFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Entity &&
                editor::State::LastSelectedObject.getSerializable() == editorSource) {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            ImGui::TreeNodeEx((void*)&idx, nodeFlags, "%s", editorSource->getName().c_str());

            if (ImGui::BeginDragDropSource()) {
                editor::DragDropObject obj {editorSource};
                ImGui::SetDragDropPayload(editor::DDPayloadID__AudioSource, &obj, sizeof(editor::DragDropObject));
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                editor::State::LastSelectedObject = editor::SelectableObject(editorSource);
            }
        }

        ImGui::TreePop();
    }
}

} // namespace editor_ui
} // namespace aderite
