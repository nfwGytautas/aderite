#include "Viewport.hpp"

#include <imgui/imgui.h>
#include <bgfx/bgfx.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/rendering/Renderer.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

void Viewport::init() {
	// TODO: Error check
	m_fbh = ::aderite::Engine::getRenderer()->getOutput();
	m_fbth = bgfx::getTexture(m_fbh, 0);
}

void Viewport::shutdown() {
}

void Viewport::render() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	if (!ImGui::Begin("Viewport")) {
		ImGui::End();
		ImGui::PopStyleVar();
		return;
	}

	if (!bgfx::isValid(m_fbh)) {
		ImGui::End();
		ImGui::PopStyleVar();
		return;
	}

	if (!bgfx::isValid(m_fbth)) {
		ImGui::End();
		ImGui::PopStyleVar();
		return;
	}

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	ImVec2 viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
	
	ImGui::Image((void*)(intptr_t)m_fbth.idx, viewportSize, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
	ImGui::PopStyleVar();
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
