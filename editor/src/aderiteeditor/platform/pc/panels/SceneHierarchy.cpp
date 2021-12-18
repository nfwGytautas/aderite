#include "SceneHierarchy.hpp"
#include <functional>

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/scene/DynamicPhysicsRegion.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/Scenery.hpp"
#include "aderite/scene/StaticPhysicsRegion.hpp"
#include "aderite/scene/Visual.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

#include "aderiteeditor/platform/pc/modals/SelectScriptModal.hpp"
#include "aderiteeditor/shared/DragDrop.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor {

static ImGuiTreeNodeFlags c_BaseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                        ImGuiTreeNodeFlags_SpanAvailWidth;

template<typename T, typename DelFn>
void genericNodeList(const std::vector<T*>& list, DelFn onDelete) {
    for (T* item : list) {
        ImGuiTreeNodeFlags leafFlags = c_BaseFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        io::SerializableObject* selectedObject = editor::State::getInstance().getSelectedObject();
        if (selectedObject == item) {
            leafFlags |= ImGuiTreeNodeFlags_Selected;
        }

        ImGui::TreeNodeEx(item->getName().c_str(), leafFlags);

        // Context menu
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                onDelete(item);

                // Quit and rerender
                ImGui::EndPopup();
                break;
            }

            ImGui::EndPopup();
        }

        // Drag drop
        DragDrop::renderSource(item);

        // Selection
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            editor::State::getInstance().setSelectedObject(item);
        }
    }
}

SceneHierarchy::SceneHierarchy() {
    m_nodes.push_back(new ObjectNode());
    m_nodes.push_back(new PhysicsRegionNode());
    m_nodes.push_back(new AudioNode());
}

SceneHierarchy::~SceneHierarchy() {
    for (ISceneHierarchyNode* node : m_nodes) {
        delete node;
    }
}

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

    ImGui::Text("%s", currentScene->getName().c_str());

    // Actual tree
    ImGui::Separator();
    for (ISceneHierarchyNode* node : m_nodes) {
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        node->render();
    }

    ImGui::End();
}

void SceneHierarchy::contextMenu() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (ImGui::BeginPopupContextWindow()) {
        for (ISceneHierarchyNode* node : m_nodes) {
            node->contextMenu();
        }

        ImGui::Separator();
        ImGui::MenuItem("(Scene)", NULL, false, false);

        if (ImGui::Selectable("Edit")) {
            editor::State::getInstance().setSelectedObject(currentScene);
        }

        ImGui::EndPopup();
    }
}

void SceneHierarchy::ObjectNode::render() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Objects")) {
        if (ImGui::TreeNode("Visuals")) {
            genericNodeList(currentScene->getVisuals(), [&currentScene](auto item) {
                currentScene->remove(item);
            });
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Scenery")) {
            genericNodeList(currentScene->getScenery(), [&currentScene](auto item) {
                currentScene->remove(item);
            });
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Entities")) {
            genericNodeList(currentScene->getEntities(), [&currentScene](auto item) {
                currentScene->remove(item);
            });
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

void SceneHierarchy::ObjectNode::contextMenu() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (ImGui::BeginMenu("Add object")) {
        if (ImGui::MenuItem("Visual")) {
            currentScene->add(new scene::Visual());
        }

        if (ImGui::MenuItem("Scenery")) {
            currentScene->add(new scene::Scenery());
        }

        if (ImGui::MenuItem("Entity")) {
            currentScene->add(new scene::Entity());
        }

        ImGui::EndMenu();
    }
}

void SceneHierarchy::AudioNode::render() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Audio")) {
        if (ImGui::TreeNode("Listeners")) {
            genericNodeList(currentScene->getAudioListeners(), [&currentScene](auto item) {
                currentScene->remove(item);
            });
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Sources")) {
            genericNodeList(currentScene->getAudioSources(), [&currentScene](auto item) {
                currentScene->remove(item);
            });
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

void SceneHierarchy::AudioNode::contextMenu() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (ImGui::BeginMenu("Audio")) {
        if (ImGui::MenuItem("Create source")) {
            audio::AudioSource* source = new audio::AudioSource();
            currentScene->add(source);
        }

        if (ImGui::MenuItem("Create listener")) {
            audio::AudioListener* listener = new audio::AudioListener();
            currentScene->add(listener);
        }

        ImGui::EndMenu();
    }
}

void SceneHierarchy::PhysicsRegionNode::render() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("Physics regions")) {
        if (ImGui::TreeNode("Static")) {
            genericNodeList(currentScene->getStaticPhysicsRegions(), [&currentScene](auto item) {
                currentScene->remove(item);
            });
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Dynamic")) {
            genericNodeList(currentScene->getDynamicPhysicsRegions(), [&currentScene](auto item) {
                currentScene->remove(item);
            });
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

void SceneHierarchy::PhysicsRegionNode::contextMenu() {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (ImGui::BeginMenu("Add physics region")) {
        if (ImGui::MenuItem("Static")) {
            currentScene->add(new scene::StaticPhysicsRegion());
        }

        if (ImGui::MenuItem("Dynamic")) {
            currentScene->add(new scene::DynamicPhysicsRegion());
        }

        ImGui::EndMenu();
    }
}

} // namespace editor
} // namespace aderite
