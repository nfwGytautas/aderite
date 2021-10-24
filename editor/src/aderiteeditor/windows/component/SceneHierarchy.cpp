#include "SceneHierarchy.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/vfs/VFS.hpp"
#include "aderiteeditor/vfs/File.hpp"
#include "aderiteeditor/windows/component/Modals.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN
			
SceneHierarchy::SceneHierarchy() {
	m_textModal = new TextInputModal();
}

SceneHierarchy::~SceneHierarchy() {
	delete m_textModal;
}

void SceneHierarchy::render() {
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
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
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Create Entity")) {
			// TODO: Make sure that this is actually unique
			currentScene->createEntity(::aderite::scene::components::MetaComponent(utility::generateString(16)));
		}

		ImGui::Separator();

		if (ImGui::Selectable("Edit")) {
			editor::State::LastSelectedObject = editor::SelectableObject(currentScene);
		}

		ImGui::EndPopup();
	}

	ImGui::Text("%s", cacheFile->getName().c_str());

	// Actual tree
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 2));
	for (auto [Entity, MetaComponent] : currentScene->getEntityRegistry().view<::aderite::scene::components::MetaComponent>().each()) {
		scene::Entity e = scene::Entity(Entity, currentScene);

		// Tree node
		bool has_children = false;

		if (has_children) {
			// If this is a Entity with children
			// TODO: TreeNodeEx (ImGui Selectable Node example)
		}
		else {
			// If this is a single Entity
			ImGuiTreeNodeFlags node_flags = base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			if (editor::State::LastSelectedObject.getType() == editor::SelectableObjectType::Entity && editor::State::LastSelectedObject.getEntity() == Entity) {
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

	ImGui::End();
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
