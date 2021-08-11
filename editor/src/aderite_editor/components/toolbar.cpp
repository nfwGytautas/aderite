#include "toolbar.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite_editor/core/state.hpp"

// OS Specific
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