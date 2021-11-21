#include "UIComponent.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "aderiteeditor/resources/EditorIcons.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor {

UIComponent::~UIComponent() {}

bool UIComponent::isProjectLoaded() const {
    return State::Project != nullptr;
}

void UIComponent::applyWindowProperties() {}

void UIComponent::renderIcon(const std::string& name, size_t width, size_t height) {
    bgfx::TextureHandle icon = EditorIcons::getInstance().getIcon(name);
    ImGui::Image((void*)(intptr_t)icon.idx, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
}

bool UIComponent::renderIconButton(const std::string& name, size_t width, size_t height) {
    bgfx::TextureHandle icon = EditorIcons::getInstance().getIcon(name);
    return ImGui::ImageButton((void*)(intptr_t)icon.idx, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0), 0);
}

void UIComponent::setCursorToCenter(const size_t elementWidth, const size_t elementHeight) {
    const ImVec2 wndSize = ImGui::GetWindowSize();
    const ImVec2 availableSize = ImGui::GetContentRegionAvail();

    ImVec2 position = availableSize;

    // Offset by element
    position.x -= elementWidth;
    position.y -= elementHeight;

    // Center
    position.x *= 0.5f;
    position.y *= 0.5f;

    // Adjust
    position.x += wndSize.x - availableSize.x;
    position.y += wndSize.y - availableSize.y;

    // Set cursor
    ImGui::SetCursorPos(position);
}

} // namespace editor
} // namespace aderite
