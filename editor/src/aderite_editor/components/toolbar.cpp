#include "toolbar.hpp"

#include <vector>
#include <filesystem>

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/core/assets/AssetManager.hpp"
#include "aderite/core/assets/object/MeshAsset.hpp"
#include "aderite/core/assets/object/shader_asset.hpp"
#include "aderite_editor/core/state.hpp"
#include "aderite_editor/core/event_router.hpp"
#include "aderite_editor/components/modals.hpp"
#include "aderite_editor/utility/file_dialog.hpp"

namespace aderite {
	namespace editor {
		namespace components {
			toolbar::toolbar() {
				m_text_modal = new modals::text_input();
			}

			toolbar::~toolbar() {
				delete m_text_modal;
			}

			void toolbar::render() {
				if (ImGui::BeginMenuBar()) {
					if (ImGui::BeginMenu("File")) {
						if (ImGui::MenuItem("New project")) {
							// Required name and directory
							std::string project_dir = file_dialog::select_directory();

							if (!project_dir.empty()) {
								// Now get the name
								m_text_modal->set_title("New project");
								m_text_modal->set_text("Project name:");
								m_text_modal->set_confirm_action([&, project_dir](const std::string& value) {
									state::Sink->new_project(project_dir, value);
								});

								m_text_modal->show();
							}
						}

						if (ImGui::MenuItem("Save project")) {
							state::Sink->save_project();
						}

						if (ImGui::MenuItem("Load project")) {
							std::string file = file_dialog::select_file("Select aderite project", { "Aderite project", "*.aproj" });

							if (!file.empty()) {
								state::Sink->load_project(file);
							}
						}

						ImGui::Separator();
						if (ImGui::MenuItem("Exit")) {
							aderite::engine::get()->request_exit();
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Project")) {
						if (ImGui::MenuItem("New scene")) {
							m_text_modal->set_title("New scene");
							m_text_modal->set_text("Scene name:");
							m_text_modal->set_confirm_action([&](const std::string& value) {
								state::Sink->new_scene(value);
							});

							m_text_modal->show();
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Optimize Raw folder")) {
							// Removes unused files from Raw folder
							std::vector<std::filesystem::path> used = {};
							for (asset::Asset* asset : *engine::get_AssetManager()) {
								if (asset->isInGroup(asset::AssetGroup::DEPENDS_ON_RAW)) {
									switch (asset->type()) {
									case asset::AssetType::MESH: {
										used.push_back(engine::get_AssetManager()->get_raw_dir() / static_cast<asset::MeshAsset*>(asset)->get_fields().SourceFile);
										break;
									}
									case asset::AssetType::SHADER: {
										used.push_back(engine::get_AssetManager()->get_raw_dir() / static_cast<asset::shader_asset*>(asset)->get_fields().VertexPath);
										used.push_back(engine::get_AssetManager()->get_raw_dir() / static_cast<asset::shader_asset*>(asset)->get_fields().FragmentPath);
										break;
									}
									case asset::AssetType::SCENE:
									case asset::AssetType::MATERIAL: {
										continue;
									}
									default:
										LOG_WARN("Unimplemented type for DEPENDS_ON_RAW asset, aborting");
										return;
									}
								}
							}

							for (auto& file : std::filesystem::recursive_directory_iterator(engine::get_AssetManager()->get_raw_dir())) {
								auto& it = std::find(used.begin(), used.end(), file.path());

								if (it == used.end()) {
									LOG_TRACE("Removing {0}", file.path().string());
									std::filesystem::remove(file.path());
								}
							}
						}

						if (ImGui::MenuItem("Recompile shaders")) {
							// Get all currently used shaders and then complete a unload/load cycle
							LOG_WARN("Not implemented feature called 'Recompile shaders'");
						}
						
						ImGui::EndMenu();
					}

					ImGui::EndMenuBar();
				}

				m_text_modal->render();
			}

		}
	}
}