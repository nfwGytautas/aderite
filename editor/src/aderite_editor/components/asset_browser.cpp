#include "asset_browser.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite_editor/core/config.hpp"
#include "aderite_editor/core/state.hpp"
#include "aderite_editor/components/component_utility.hpp"

#include "aderite/core/assets/object/shader_asset.hpp"

namespace aderite {
	namespace editor {
		namespace components {
			constexpr int c_status_button_height = 20.0f;

			template<typename T>
			T* create_item(const std::filesystem::path& root, const std::string& default_name, const std::string& extension) {
				std::filesystem::path path_name = root / (default_name + extension);
				int it = 1;
				while (engine::get_asset_manager()->has(path_name.string())) {
					path_name = root / (default_name + " " + std::to_string(it) + extension);
				}
				T* s = engine::get_asset_manager()->create<T>(path_name.string());
				engine::get_asset_manager()->save_asset(s);
				return s;
			}

			/**
			 * @brief Recusively collects all files from directory into a vector of paths, pushing paths relative to one point
			*/
			void recursive_fs_collect(std::vector<std::filesystem::path>& entries, std::filesystem::path dir, std::filesystem::path& relativeTo) {
				for (auto& dir_entry : std::filesystem::directory_iterator(dir)) {
					const auto& it = dir_entry.path();

					if (std::filesystem::is_directory(it)) {
						recursive_fs_collect(entries, it, relativeTo);
					}
					else {
						entries.push_back(std::filesystem::relative(it, relativeTo));
					}
				}
			}

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
				static std::string current_renaming = "";
				static bool appearing = false;
				static std::string rename_value = "";
				static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;

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
					// Context menu
					if (ImGui::BeginPopupContextWindow())
					{
						if (ImGui::MenuItem("New folder")) {
							std::filesystem::path dir = res_dir / m_current_dir / "New folder";
							int dir_it = 1;
							while (std::filesystem::exists(dir)) {
								dir = res_dir / m_current_dir / ("New folder " + std::to_string(dir_it++));
							}
							std::filesystem::create_directory(dir);
							resolve_fs();
						}

						ImGui::Separator();

						if (ImGui::BeginMenu("New asset")) {
							if (ImGui::MenuItem("Scene")) {
								create_item<scene::scene>(m_current_dir, "New scene", ".scene");
							}

							if (ImGui::MenuItem("Mesh")) {

							}

							if (ImGui::MenuItem("Material")) {

							}

							if (ImGui::MenuItem("Shader")) {
								create_item<asset::shader_asset>(m_current_dir, "New shader", ".shader");
							}

							ImGui::EndMenu();
						}

						ImGui::EndPopup();
					}

					ImGui::TableNextRow();
					for (fs_node& node : m_it_nodes) {
						ImGui::TableNextColumn();

						//ImGui::Text("%ld %s %s", node.Type, node.Name.c_str(), node.Stem.c_str());
						if (ImGui::Button(node.Name.c_str(), ImVec2(cellSize, cellSize))) {

						}

						// Context menu
						if (ImGui::BeginPopupContextItem())
						{
							std::filesystem::path path = res_dir / node.Name;
							if (path != engine::get_asset_manager()->get_raw_dir()) {
								if (ImGui::MenuItem("Delete")) {
									// TODO: Confirmation window
									delete_item(path);
									resolve_fs();
								}
							}

							ImGui::EndPopup();
						}

						if (ImGui::BeginDragDropSource()) {
							// Typed
							std::string target = "";
							switch (node.Type) {
							case fs_node_type::SCENE:
								target = DDPayloadID_SceneAsset;
								break;
							case fs_node_type::SHADER:
								target = DDPayloadID_ShaderAsset;
								break;
							case fs_node_type::MATERIAL:
								target = DDPayloadID_MaterialAsset;
								break;
							case fs_node_type::MESH:
								target = DDPayloadID_MeshAsset;
								break;
							}

							if (!target.empty()) {
								ImGui::SetDragDropPayload(target.c_str(), node.Name.c_str(), node.Name.size() + 1);
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

							switch (node.Type) {
							case fs_node_type::DIRECTORY:
							{
								m_current_dir = m_current_dir / node.Stem;
								resolve_fs();
								break;
							}
							case fs_node_type::SCENE:
							{
								// TODO: Error check
								// TODO: Move to editor?
								scene::scene* scene = static_cast<scene::scene*>(engine::get_asset_manager()->get_by_name(node.Name));
								engine::get_scene_manager()->set_active(scene);
								break;
							}
							case fs_node_type::MATERIAL:
							case fs_node_type::SHADER:
							case fs_node_type::MESH:
							{
								state::Sink->selected_asset_changed(engine::get_asset_manager()->get_by_name(node.Name));
								break;
							}
							}
						}

						if (current_renaming != node.Name) {
							if (ImGui::Selectable(node.Stem.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
								if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
									rename_value = node.Stem;
									current_renaming = node.Name;
									appearing = true;
								}
							}
						}
						else {
							if (DynamicInputText("##rename", &rename_value, edit_flags)) {
								current_renaming = "";

								if (!rename_value.empty()) {
									rename_item(node.Name, rename_value + node.Extension);
									resolve_fs();
									break;
								}
							}

							if (!ImGui::IsItemActive() && !appearing) {
								current_renaming = "";
							}

							if (appearing) {
								ImGui::SetKeyboardFocusHere();
								appearing = false;
							}
						}
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
					const auto& name = std::filesystem::relative(dir_entry.path(), res_dir);
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
						else if (ext == ".shader") {
							type = fs_node_type::SHADER;
						}
						else if (ext == ".mesh") {
							type = fs_node_type::MESH;
						}
						else if (ext == ".material") {
							type = fs_node_type::MATERIAL;
						}
					}

					// Resolve extension
					m_it_nodes.push_back({type, stem, name.string(), ext.string()});
				}
			}

			void asset_browser::delete_item(std::filesystem::path path) {
				if (path == engine::get_asset_manager()->get_raw_dir()) {
					LOG_WARN("Tried to delete Raw directory");
					return;
				}

				if (std::filesystem::is_directory(path)) {
					// Scan entire folder for assets and delete from asset manager
					for (auto& dir_entry : std::filesystem::directory_iterator(path)) {
						const auto& it = dir_entry.path();
						delete_item(it);
					}
				}
				else {
					// Remove from asset manager
					// No need to check, cause all assets are read on editors
					engine::get_asset_manager()->unload(std::filesystem::relative(path, engine::get_asset_manager()->get_res_dir()).string());
				}

				// TODO: Check error
				std::filesystem::remove(path);
			}

			void asset_browser::rename_item(const std::string& prevName, const std::string& newName) {
				auto& res_dir = engine::get_asset_manager()->get_res_dir();
				std::filesystem::path parent = std::filesystem::path(prevName).parent_path();
				std::string fullNewName = (parent / newName).string();

				if (std::filesystem::exists(res_dir / fullNewName)) {
					// Conflict
					return;
				}

				if (std::filesystem::is_directory(res_dir / prevName)) {
					// Recursively rename

					// Collect all paths
					std::vector<std::filesystem::path> entries = {};
					recursive_fs_collect(entries, res_dir / prevName, res_dir / prevName);

					// Rename directory
					std::filesystem::rename(res_dir / prevName, res_dir / fullNewName);

					// Rename all assets
					for (auto& path : entries) {
						asset::asset_base* asset = engine::get_asset_manager()->get_by_name((prevName / path).string());
						if (asset != nullptr) {
							asset->set_name((fullNewName / path).string());

							// Validate project state
							state::Project->validate();
						}
					}
				}
				else {
					// This is most likely unnecessary
					if (!engine::get_asset_manager()->has(fullNewName)) {
						asset::asset_base* asset = engine::get_asset_manager()->get_by_name(prevName);
						if (asset != nullptr) {
							asset->set_name(fullNewName);
							
							// Move file
							std::filesystem::rename(res_dir / prevName, res_dir / fullNewName);

							// Validate project state
							state::Project->validate();
						}
					}
				}
			}

		}
	}
}