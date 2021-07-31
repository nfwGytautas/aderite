#include "viewport.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			void viewport::init() {
				m_viewport = dynamic_cast<aderite::render_backend::opengl::gl_fbo*>(aderite::fbo::create({ 800, 600 }));
				engine::get_renderer()->set_default_target(m_viewport);
			}

			void viewport::shutdown() {
				delete m_viewport;
			}

			void viewport::render() {
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
				//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				ImGui::Begin("Viewport");
				ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
				ImVec2 viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
				ImGui::Image((ImTextureID)m_viewport->get_ta(), viewportSize, ImVec2(0, 1), ImVec2(1, 0));
				ImGui::End();
				ImGui::PopStyleVar();
			}

		}
	}
}