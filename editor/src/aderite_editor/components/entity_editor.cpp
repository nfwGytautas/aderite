#include "entity_editor.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/core/scene/scene.hpp"
#include "aderite/core/assets/asset_manager.hpp"
#include "aderite/core/assets/object/mesh_asset.hpp" 
#include "aderite/core/assets/object/material_asset.hpp" 
#include "aderite_editor/core/state.hpp"
#include "aderite_editor/core/config.hpp"
#include "aderite_editor/core/event_router.hpp"
#include "aderite_editor/components/component_utility.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			template<typename Component, typename RenderFn>
			bool render_component(const std::string& label, ::aderite::scene::entity& entity, RenderFn renderFn) {
				const ImGuiTreeNodeFlags treeNodeFlags = /*ImGuiTreeNodeFlags_DefaultOpen | */ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

				if (entity.has_component<Component>()) {
					auto& component = entity.get_component<Component>();
					ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
					float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
					
					//ImGui::Separator();

					bool open = ImGui::TreeNodeEx((void*)typeid(Component).hash_code(), treeNodeFlags, label.c_str());
					ImGui::PopStyleVar();

					ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
					if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
						ImGui::OpenPopup("ComponentSettings");
					}

					bool removeComponent = false;
					if (ImGui::BeginPopup("ComponentSettings")) {
						if (ImGui::MenuItem("Remove component"))
							removeComponent = true;

						ImGui::EndPopup();
					}

					if (open) {
						renderFn(component);
						ImGui::TreePop();
					}

					if (removeComponent) {
						entity.remove_component<Component>();
					}

					return true;
				}

				return false;
			}

			static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
			{
				ImGuiIO& io = ImGui::GetIO();
				auto boldFont = io.Fonts->Fonts[0];

				ImGui::PushID(label.c_str());

				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, columnWidth);
				ImGui::Text(label.c_str());
				ImGui::NextColumn();

				ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
				ImGui::PushFont(boldFont);
				if (ImGui::Button("X", buttonSize))
					values.x = resetValue;
				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ImGui::PushFont(boldFont);
				if (ImGui::Button("Y", buttonSize))
					values.y = resetValue;
				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				ImGui::PushFont(boldFont);
				if (ImGui::Button("Z", buttonSize))
					values.z = resetValue;
				ImGui::PopFont();
				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();

				ImGui::PopStyleVar();

				ImGui::Columns(1);

				ImGui::PopID();
			}

			entity_editor::entity_editor()
			{}

			entity_editor::~entity_editor()
			{}

			void entity_editor::render() {
				static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
				static bool renaming = false;
				static bool appearing = false;
				static std::string rename_value = "";

				if (!ImGui::Begin("Entity editor")) {
					ImGui::End();
					return;
				}

				if (m_selected_entity == scene::entity::null()) {
					ImGui::Text("Select entity from scene view");
					ImGui::End();
					return;
				}

				// Component list
				if (!m_selected_entity.has_component<::aderite::scene::components::meta>()) {
					// This shouldn't happen
					LOG_ERROR("Entity without meta information selected");
					state::Sink->selected_entity_changed(scene::entity::null());
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
							appearing = true;
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

					if (!ImGui::IsItemActive() && !appearing) {
						renaming = false;
					}

					if (appearing) {
						ImGui::SetKeyboardFocusHere();
						appearing = false;
					}
				}

				// Other components as tree nodes that can be collapsed
				ImGui::Separator();

				bool hasTransform = render_component<::aderite::scene::components::transform>("Transform", m_selected_entity, 
					[](::aderite::scene::components::transform& c) {
					DrawVec3Control("Position", c.Position);
					glm::vec3 rotation = glm::degrees(c.Rotation);
					DrawVec3Control("Rotation", rotation);
					c.Rotation = glm::radians(rotation);
					DrawVec3Control("Scale", c.Scale, 1.0f);
				});

				bool hasMeshRenderer = render_component<::aderite::scene::components::mesh_renderer>("Mesh renderer", m_selected_entity,
					[](::aderite::scene::components::mesh_renderer& c) {

					if (ImGui::BeginTable("MeshRendererTable", 2)) {
						ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 70.0f);
						ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Mesh");

						ImGui::TableSetColumnIndex(1);
						ImGui::PushItemWidth(-FLT_MIN);

						if (c.MeshHandle) {
							ImGui::Button(c.MeshHandle->get_name().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
						}
						else {
							ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
						}

						if (ImGui::BeginDragDropTarget()) {
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DDPayloadID_MeshAsset)) {
								std::string name = static_cast<const char*>(payload->Data);
								asset::asset_base* asset = engine::get_asset_manager()->get_by_name(name);
								if (asset) {
									c.MeshHandle = static_cast<asset::mesh_asset*>(asset);
								}
							}

							ImGui::EndDragDropTarget();
						}

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Material");

						ImGui::TableSetColumnIndex(1);
						ImGui::PushItemWidth(-FLT_MIN);

						if (c.MaterialHandle) {
							ImGui::Button(c.MaterialHandle->get_name().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
						}
						else {
							ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
						}

						if (ImGui::BeginDragDropTarget()) {
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DDPayloadID_MaterialAsset)) {
								std::string name = static_cast<const char*>(payload->Data);
								asset::asset_base* asset = engine::get_asset_manager()->get_by_name(name);
								if (asset) {
									c.MaterialHandle = static_cast<asset::material_asset*>(asset);
								}
							}

							ImGui::EndDragDropTarget();
						}

						ImGui::EndTable();
					}
				});

				bool hasAll = hasTransform && hasMeshRenderer;

				if (!hasAll) {
					ImGui::Separator();

					float width = ImGui::GetContentRegionAvail().x * 0.4855f;
					ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
					if (ImGui::Button("Add component", ImVec2(width, 0.0f))) {
						ImGui::OpenPopup("AddComponent");
					}

					if (ImGui::BeginPopup("AddComponent")) {
						if (!hasTransform && ImGui::MenuItem("Transform")) {
							m_selected_entity.add_component<::aderite::scene::components::transform>();
							ImGui::CloseCurrentPopup();
						}

						if (!hasMeshRenderer && ImGui::MenuItem("Mesh renderer")) {
							m_selected_entity.add_component<::aderite::scene::components::mesh_renderer>();
							ImGui::CloseCurrentPopup();
						}

						ImGui::EndPopup();
					}
				}

				ImGui::End();
			}

			void entity_editor::set_active_entity(scene::entity& entity) {
				m_selected_entity = entity;
			}

		}
	}
}
