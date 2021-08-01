#include "scene_view.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/core/scene/scene.hpp"
#include "aderite/utility/random.hpp"
#include "aderite_editor/core/event_router.hpp"
#include "aderite_editor/components/utility.hpp"

namespace aderite {
	namespace editor {
		namespace components {
			
			scene_view::scene_view() {
				m_text_modal = new modals::text_input();
			}

			scene_view::~scene_view() {
				delete m_text_modal;
			}

			void scene_view::set_active_entity(scene::entity& entity) {
				m_selected_entity = entity;
			}

			void scene_view::render() {
				static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

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

				// Context menu
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::Selectable("Create entity")) {
						// TODO: Make sure that this is actually unique
						event_router::Sink->create_entity(random::generate_string(16));
					}

					ImGui::EndPopup();
				}

				ImGui::Text("%s", currentScene->get_name().c_str());

				// Actual tree
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 2));
				for (auto [entity, meta] : currentScene->get_entity_registry().view<::aderite::scene::components::meta>().each()) {
					scene::entity e = scene::entity(entity, currentScene);

					// Tree node
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

						ImGui::TreeNodeEx((void*)(intptr_t)(uint32_t)entity, node_flags, "%s", meta.Name.c_str());

						if (ImGui::IsItemClicked()) {
							event_router::Sink->selected_entity_changed(e);
						}
					}

					// Context menu
					if (ImGui::BeginPopupContextItem()) {
						event_router::Sink->selected_entity_changed(e);

						if (ImGui::Selectable("Delete")) {
							event_router::Sink->destroy_entity(e);
						}

						ImGui::EndPopup();
					}
				}
				ImGui::PopStyleVar();

				ImGui::End();
			}

		}
	}
}