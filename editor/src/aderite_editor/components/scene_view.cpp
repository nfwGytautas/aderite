#include "scene_view.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/random.hpp"
#include "aderite/core/scene/scene.hpp"
#include "aderite/core/scene/SceneManager.hpp"
#include "aderite_editor/core/state.hpp"
#include "aderite_editor/core/event_router.hpp"
#include "aderite_editor/components/modals.hpp"
#include "aderite_editor/components/component_utility.hpp"

namespace aderite {
	namespace editor {
		namespace components {
			
			scene_view::scene_view() {
				m_text_modal = new modals::text_input();
			}

			scene_view::~scene_view() {
				delete m_text_modal;
			}

			void scene_view::set_active_entity(scene::Entity& Entity) {
				m_selected_entity = Entity;
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
					if (ImGui::Selectable("Create Entity")) {
						// TODO: Make sure that this is actually unique
						state::Sink->create_entity(random::generate_string(16));
					}

					ImGui::EndPopup();
				}

				ImGui::Text("%s", currentScene->getName().c_str());

				// Actual tree
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 2));
				for (auto [Entity, MetaComponent] : currentScene->get_entity_registry().view<::aderite::scene::components::MetaComponent>().each()) {
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

						if (m_selected_entity == Entity) {
							node_flags |= ImGuiTreeNodeFlags_Selected;
						}

						ImGui::TreeNodeEx((void*)(intptr_t)(uint32_t)Entity, node_flags, "%s", MetaComponent.Name.c_str());

						if (ImGui::IsItemClicked()) {
							state::Sink->selected_entity_changed(e);
						}
					}

					// Context menu
					if (ImGui::BeginPopupContextItem()) {
						state::Sink->selected_entity_changed(e);

						if (ImGui::Selectable("Delete")) {
							state::Sink->destroy_entity(e);
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