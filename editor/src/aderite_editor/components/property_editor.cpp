#include "property_editor.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/core/scene/scene.hpp"
#include "aderite_editor/core/event_router.hpp"
#include "aderite_editor/components/utility.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			property_editor::property_editor()
			{}

			property_editor::~property_editor()
			{}

			void property_editor::render() {
				static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
				static bool renaming = false;
				static std::string rename_value = "";

				if (!ImGui::Begin("Property editor")) {
					ImGui::End();
					return;
				}

				if (m_selected_entity == scene::entity::null()) {
					ImGui::End();
					return;
				}

				// Component list
				if (!m_selected_entity.has_component<::aderite::scene::components::meta>()) {
					// This shouldn't happen
					LOG_ERROR("Entity without meta information selected");
					event_router::Sink->selected_entity_changed(scene::entity::null());
					ImGui::End();
					return;
				}

				// Meta
				auto& meta = m_selected_entity.get_component<::aderite::scene::components::meta>();

				if (!renaming) {
					if (ImGui::Selectable(meta.Name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
							rename_value = meta.Name;
							renaming = true;
						}
					}
				}
				else {
					if (DynamicInputText("##rename", &rename_value, edit_flags)) {
						renaming = false;

						if (!rename_value.empty()) {
							// TODO: Check for name conflict
							meta.Name = rename_value;
						}
					}
				}

				// Other components as tree nodes that can be collapsed
				ImGui::Separator();



				ImGui::Separator();

				float width = ImGui::GetContentRegionAvail().x * 0.4855f;
				ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
				if (ImGui::Button("Add component", ImVec2(width, 0.0f))) {
					ImGui::OpenPopup("AddComponent");
				}

				if (ImGui::BeginPopup("AddComponent"))
				{
					/*if (ImGui::MenuItem("Tag"))
					{
						
						ImGui::CloseCurrentPopup();
					}*/

					ImGui::EndPopup();
				}

				ImGui::End();
			}

			void property_editor::set_active_entity(scene::entity& entity) {
				m_selected_entity = entity;
			}

		}
	}
}
