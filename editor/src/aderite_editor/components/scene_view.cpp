#include "scene_view.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"

#include "aderite/core/scene/scene.hpp"

namespace aderite {
	namespace editor {
		namespace components {
			void scene_view::set_active_entity(scene::entity* entity) {
				m_selected_entity = *entity;
			}

			void scene_view::render_entity(size_t entity) {
				static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
				bool has_children = false;

				if (has_children) {
					// If this is a entity with children
					// TODO: TreeNodeEx (ImGui Selectable Node example)
				}
				else {
					// If this is a single entity
					ImGuiTreeNodeFlags node_flags = base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

					if (m_selected_entity == entity) {
						node_flags |= ImGuiTreeNodeFlags_Selected;
					}

					ImGui::TreeNodeEx((void*)(intptr_t)entity, node_flags, "Entity: %ld", entity);
					if (ImGui::IsItemClicked()) {
						aderite::engine::get_editor()->on_selected_entity_changed(&entity);
					}
				}
			}

			void scene_view::render() {
				if (!ImGui::Begin("Scene hierarchy")) {
					ImGui::End();
					return;
				}

				// Root node name is the scene name
				scene::scene* currentScene = aderite::engine::get_scene_manager()->current_scene();

				if (!currentScene) {
					ImGui::End();
					return;
				}

				ImGui::Text("%s", currentScene->get_name().c_str());

				// Context menu

				// Actual tree
				//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 2));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 2));
				// TODO: Replace with scene entities
				for (size_t i = 0; i < 5; i++) {
					render_entity(i);
				}
				ImGui::PopStyleVar();

				ImGui::End();
			}

		}
	}
}