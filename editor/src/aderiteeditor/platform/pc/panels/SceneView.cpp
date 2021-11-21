#include "SceneView.hpp"

#include <bgfx/bgfx.h>
#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/shared/EditorCamera.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor {

bool SceneView::init() {
    return true;
}

void SceneView::shutdown() {}

void SceneView::render() {
    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0, 0});
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    if (!ImGui::Begin("Scene view")) {
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    if (!bgfx::isValid(editor::State::DebugRenderHandle)) {
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    m_fbth = bgfx::getTexture(editor::State::DebugRenderHandle, 0);
    if (!bgfx::isValid(m_fbth)) {
        ImGui::End();
        ImGui::PopStyleVar();
        return;
    }

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    ImVec2 viewportSize = {viewportPanelSize.x, viewportPanelSize.y};
    m_size.x = viewportPanelSize.x;
    m_size.y = viewportPanelSize.y;
    editor::State::EditorCamera->onViewportResize(m_size);

    ImGui::Image((void*)(intptr_t)m_fbth.idx, viewportSize, ImVec2(1, 0), ImVec2(0, 1));

    ImGui::End();
    ImGui::PopStyleVar();
}

void SceneView::onSceneChanged(scene::Scene* scene) {
    // TODO: Error check
    m_fbth = bgfx::getTexture(editor::State::DebugRenderHandle, 0);
    editor::State::EditorCamera->onViewportResize(m_size);
}

} // namespace editor_ui
} // namespace aderite
