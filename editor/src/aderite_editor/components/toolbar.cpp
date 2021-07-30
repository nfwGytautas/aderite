#include "toolbar.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"

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
							
						}

						if (ImGui::MenuItem("Save project")) {
							
						}

						if (ImGui::MenuItem("Load project")) {
							
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
								LOG_INFO("New scene name: {0}", value);
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