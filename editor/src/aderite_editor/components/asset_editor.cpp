#include "asset_editor.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/core/scene/scene.hpp"
#include "aderite_editor/core/state.hpp"
#include "aderite_editor/components/component_utility.hpp"
#include "aderite_editor/core/config.hpp"
#include "aderite_editor/utility/file_dialog.hpp"
#include "aderite_editor/utility/utility.hpp"

#include "aderite/core/assets/object/shader_asset.hpp"

namespace aderite {
	namespace editor {
		namespace components {
			asset_editor::asset_editor()
			{}

			asset_editor::~asset_editor()
			{}

			void asset_editor::render() {
				static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
				static bool renaming = false;
				static bool appearing = false;
				static std::string rename_value = "";

				if (!ImGui::Begin("Asset editor")) {
					ImGui::End();
					return;
				}

				if (m_selected_asset == nullptr) {
					ImGui::End();
					return;
				}

				if (!renaming) {
					if (ImGui::Selectable(m_selected_asset->get_name().c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
							rename_value = m_selected_asset->get_name();
							renaming = true;
							appearing = true;
						}
					}
				}
				else {
					if (DynamicInputText("##rename", &rename_value, edit_flags)) {
						renaming = false;

						if (!rename_value.empty()) {
							if (engine::get_asset_manager()->has(rename_value)) {
								std::filesystem::rename(engine::get_asset_manager()->get_res_dir() / m_selected_asset->get_name(), engine::get_asset_manager()->get_res_dir() / rename_value);
								m_selected_asset->set_name(rename_value);
							}
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

				// Type specific
				ImGui::Separator();

				// Type switch
				switch (m_selected_asset->type()) {
				case asset::asset_type::SHADER:
				{
					shader_render();
					break;
				}
				default:
					ImGui::Text("Unsupported asset type");
				}

				ImGui::End();
			}

			void asset_editor::set_active_asset(asset::asset_base* asset) {
				m_selected_asset = asset;
			}
			
			void asset_editor::shader_render() {
				asset::shader_asset* shader = static_cast<asset::shader_asset*>(m_selected_asset);
				asset::shader_asset::fields& finfo = shader->get_fields_mutable();

				if (ImGui::BeginTable("ShaderEditTable", 3)) {
					ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
					ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
					ImGui::TableSetupColumn("Add", ImGuiTableColumnFlags_WidthFixed, 20.0f);

					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Vertex program");

					ImGui::TableSetColumnIndex(1);
					ImGui::PushItemWidth(-FLT_MIN);

					if (!finfo.VertexPath.empty()) {
						ImGui::Button(finfo.VertexPath.c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
					}
					else {
						ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
					}

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DDPayloadID_RawData)) {

						}

						ImGui::EndDragDropTarget();
					}

					ImGui::TableSetColumnIndex(2);
					if (ImGui::Button("+###VertexSelect")) {
						std::string file = file_dialog::select_file("Select vertex shader program source file");

						if (!file.empty()) {
							// Copy file to Raw
							std::filesystem::path filename = std::filesystem::path(file).filename();
							std::filesystem::path raw_dst = utility::make_unique_path(engine::get_asset_manager()->get_raw_dir() / filename);
							std::filesystem::copy_file(file, raw_dst);
							finfo.VertexPath = raw_dst.filename().string();
						}
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Fragment program");

					ImGui::TableSetColumnIndex(1);
					ImGui::PushItemWidth(-FLT_MIN);

					if (!finfo.FragmentPath.empty()) {
						ImGui::Button(finfo.FragmentPath.c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
					}
					else {
						ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
					}

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DDPayloadID_RawData)) {

						}

						ImGui::EndDragDropTarget();
					}

					ImGui::TableSetColumnIndex(2);
					if (ImGui::Button("+###FragmentSelect")) {
						std::string file = file_dialog::select_file("Select fragment shader program source file");

						if (!file.empty()) {
							std::filesystem::path filename = std::filesystem::path(file).filename();
							std::filesystem::path raw_dst = utility::make_unique_path(engine::get_asset_manager()->get_raw_dir() / filename);
							std::filesystem::copy_file(file, raw_dst);
							finfo.FragmentPath = raw_dst.filename().string();
						}
					}

					// TODO: Try compile button

					ImGui::EndTable();
				}
			}
		}
	}
}
