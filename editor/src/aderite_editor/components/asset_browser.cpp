#include "asset_browser.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite_editor/core/config.hpp"
#include "aderite_editor/core/state.hpp"

namespace aderite {
	namespace editor {
		namespace components {
			constexpr int c_status_button_height = 20.0f;

			/**
			 * @brief Drag and drop handler for asset browser elements
			 * @param path New path to move to
			 * @return True if object was moved, false otherwise
			*/
			bool target_handler(const std::filesystem::path& path) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DDPayloadID_GenericAsset)) {
					const char* asset_name = static_cast<const char*>(payload->Data);

					// Get asset from manager and move it to the new directory
					asset::asset_base* asset = engine::get_asset_manager()->get_by_name(asset_name);
					if (asset) {
						std::string oldPath = asset->get_name();
						std::string newPath = (path / std::filesystem::path(asset->get_name()).filename()).string();

						if (newPath == asset->get_name()) {
							return false;
						}

						if (engine::get_asset_manager()->has(newPath)) {
							LOG_WARN("Asset {0} already exists", newPath);
							return false;
						}

						asset->set_name(newPath);

						if (asset->type() == asset::asset_type::SCENE) {
							if (state::Project->get_active_scene() == asset_name) {
								state::Sink->save_project();
							}
						}

						// Move file
						std::filesystem::rename(
							engine::get_asset_manager()->get_res_dir() / oldPath, 
							engine::get_asset_manager()->get_res_dir() / newPath);

						return true;
					}
				}

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DDPayloadID_Directory)) {
					const char* dir = static_cast<const char*>(payload->Data);

					// TODO: Move entire directory and all it's assets
					LOG_WARN("Directory moving is not implemented!");
				}

				return false;
			}

			asset_browser::asset_browser()
			{}

			asset_browser::~asset_browser()
			{}

			void asset_browser::render(){
				static int fcnt = 0;
				static float padding = 16.0f;
				static float thumbnailSize = 128.0f;
				static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY;
				static float cellSize = thumbnailSize + padding;

				if (!ImGui::Begin("Asset browser")) {
					ImGui::End();
					return;
				}

				auto& res_dir = engine::get_asset_manager()->get_res_dir();

				if (!std::filesystem::exists(res_dir)) {
					ImGui::End();
					return;
				}

				if (fcnt == m_update_interval) {
					// Check for directory updates
					resolve_fs();
					fcnt = 0;
				}
				else {
					fcnt++;
				}

				// Display
				
				// Back button
				if (ImGui::Button("<", ImVec2(0.0f , c_status_button_height))) {
					m_current_dir = m_current_dir.parent_path();
					resolve_fs();
				}
				ImGui::SameLine(0.0f, 0.0f);
				ImGui::Text("|");
				ImGui::SameLine(0.0f, 0.5f);

				// Path bar
				
				// The root
				if (ImGui::Button("/", ImVec2(0.0f, c_status_button_height))) {
					m_current_dir = "";
					resolve_fs();
				}

				if (ImGui::BeginDragDropTarget()) {
					if (target_handler("")) {
						resolve_fs();
					}
					ImGui::EndDragDropTarget();
				}

				std::filesystem::path it_path = "";
				for (std::vector<std::string>::reverse_iterator i = m_path_nodes.rbegin(); i != m_path_nodes.rend(); ++i) {
					it_path = it_path / *i;
					
					ImGui::SameLine(0.0f, 1.0f);
					if (ImGui::Button((*i + "/").c_str(), ImVec2(0.0f, c_status_button_height))) {
						m_current_dir = it_path;
						resolve_fs();
						break;
					}

					if (ImGui::BeginDragDropTarget()) {
						if (target_handler(it_path)) {
							resolve_fs();
						}
						ImGui::EndDragDropTarget();
					}
				}

				ImGui::Separator();

				// Files
				float panelWidth = ImGui::GetContentRegionAvail().x;
				int columnCount = (int)(panelWidth / cellSize);

				if (ImGui::BeginTable("AssetBrowserTable", columnCount, flags)) {
					ImGui::TableNextRow();
					for (fs_node& node : m_it_nodes) {
						ImGui::TableNextColumn();

						//ImGui::Text("%ld %s %s", node.Type, node.Name.c_str(), node.Stem.c_str());
						if (ImGui::Button(node.Name.c_str(), ImVec2(cellSize, cellSize))) {

						}

						if (ImGui::BeginDragDropSource()) {
							// Typed
							switch (node.Type) {
							case fs_node_type::SCENE:
								ImGui::SetDragDropPayload(DDPayloadID_SceneAsset, node.Name.c_str(), node.Name.size() + 1);
								break;
							}
							
							// Generic
							if (node.Type != fs_node_type::DIRECTORY) {
								ImGui::SetDragDropPayload(DDPayloadID_GenericAsset, node.Name.c_str(), node.Name.size() + 1);
							}
							else {
								ImGui::SetDragDropPayload(DDPayloadID_Directory, node.Name.c_str(), node.Name.size() + 1);
							}

							ImGui::EndDragDropSource();
						}

						if (node.Type == fs_node_type::DIRECTORY) {
							if (ImGui::BeginDragDropTarget()) {
								if (target_handler(node.Name)) {
									resolve_fs();
								}
								ImGui::EndDragDropTarget();
							}
						}

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
							if (node.Type == fs_node_type::DIRECTORY) {
								m_current_dir = m_current_dir / node.Stem;
								resolve_fs();
							}
						}

						ImGui::TextWrapped(node.Stem.c_str());
					}

					ImGui::EndTable();
				}

				ImGui::End();
			}

			void asset_browser::resolve_fs() {
				// Get all files in the current directory
				auto& res_dir = engine::get_asset_manager()->get_res_dir();
				m_it_nodes.clear();
				m_path_nodes.clear();

				// Resolve path nodes
				std::filesystem::path it_path = m_current_dir;
				while (!it_path.empty()) {
					m_path_nodes.push_back(it_path.stem().string());
					it_path = it_path.parent_path();
				}

				for (auto& dir_entry : std::filesystem::directory_iterator(res_dir / m_current_dir)) {
					const auto& name = std::filesystem::relative(dir_entry.path(), res_dir).string();
					const auto& stem = dir_entry.path().stem().string();
					const auto& ext = dir_entry.path().extension();
					fs_node_type type = fs_node_type::UNKNOWN;

					if (dir_entry.is_directory()) {
						type = fs_node_type::DIRECTORY;
					}
					else {
						if (ext == ".scene") {
							type = fs_node_type::SCENE;
						}
					}

					// Resolve extension
					m_it_nodes.push_back({type, stem, name});
				}
			}

		}
	}
}