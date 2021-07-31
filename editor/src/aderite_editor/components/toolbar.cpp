#include "toolbar.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite_editor/core/event_router.hpp"

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
									event_router::Sink->new_project(project_dir, value);
								});

								m_text_modal->show();
							}
						}

						if (ImGui::MenuItem("Save project")) {
							event_router::Sink->save_project();
						}

						if (ImGui::MenuItem("Load project")) {
							std::string file = file_dialog::select_file("Select aderite project", { "Aderite project", "*.aproj" });

							if (!file.empty()) {
								event_router::Sink->load_project(file);
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
								event_router::Sink->new_scene(value);
							});

							m_text_modal->show();
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