#include "ImGui.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace aderite {
namespace utility {

static int DynamicInputTextCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        std::string* str = (std::string*)data->UserData;
        // IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    return 0;
}

bool DynamicInputText(const char* label, std::string* value, ImGuiInputTextFlags flags) {
    return ImGui::InputText(label, (char*)value->c_str(), value->capacity() + 1, flags, DynamicInputTextCallback, value);
}

bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth) {
    bool altered = false;

    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize)) {
        values.x = resetValue;
        altered = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f")) {
        altered = true;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize)) {
        values.y = resetValue;
        altered = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f")) {
        altered = true;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize)) {
        values.z = resetValue;
        altered = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f")) {
        altered = true;
    }
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    ImGui::Dummy(ImVec2(0.0f, 2.5f));

    return altered;
}

void ImSpinner(const char* label, const float indicator_radius, const int circle_count, const float speed, glm::vec2 size) {
    const auto style = ImGui::GetStyle();

    const ImVec4& main_color = ImVec4(15.0f / 255.0f, 135.0f / 255.0f, 250.0f / 255.0f, 1.0f);
    const ImVec4& backdrop_color = style.Colors[ImGuiCol_ButtonActive];

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(label);

    const ImVec2 region = ImGui::GetContentRegionAvail();
    const ImVec2 pos = ImVec2(window->DC.CursorPos.x + ((region.x - indicator_radius) / 2.0f), window->DC.CursorPos.y + ((region.x - indicator_radius) / 3.0f));
    const float circle_radius = indicator_radius / 10.0f;
    //const ImRect bb(pos, ImVec2(pos.x + indicator_radius * 2.0f, pos.y + indicator_radius * 2.0f));
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) {
        return;
    }

    const float t = g.Time;
    const auto degree_offset = 2.0f * IM_PI / circle_count;
    for (int i = 0; i < circle_count; ++i) {
        const auto x = indicator_radius * std::sin(degree_offset * i);
        const auto y = indicator_radius * std::cos(degree_offset * i);
        const auto growth = std::max(0.0f, std::sin(t * speed - i * degree_offset));
        ImVec4 color;
        color.x = main_color.x * growth + backdrop_color.x * (1.0f - growth);
        color.y = main_color.y * growth + backdrop_color.y * (1.0f - growth);
        color.z = main_color.z * growth + backdrop_color.z * (1.0f - growth);
        color.w = 1.0f;
        window->DrawList->AddCircleFilled(ImVec2(pos.x + indicator_radius + x - 5, pos.y + indicator_radius - y),
                                          circle_radius + growth * circle_radius, ImGui::GetColorU32(color));
    }
}

bool InlineRename::renderUI() {
    static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank |
                                            ImGuiInputTextFlags_EnterReturnsTrue;
    bool renamed = false;

    if (!m_renaming) {
        if (ImGui::Selectable(m_initial.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                m_initial = "";
                m_renaming = true;
                m_appearing = true;
            }
        }
    } else {
        if (DynamicInputText("##rename", &m_value, edit_flags)) {
            m_renaming = false;

            if (m_value.empty()) {
                m_value = m_initial;
            }

            renamed = true;
        }

        if (!ImGui::IsItemActive() && !m_appearing) {
            m_renaming = false;
        }

        if (m_appearing) {
            ImGui::SetKeyboardFocusHere();
            m_appearing = false;
        }
    }

    return renamed;
}

bool InlineRename::renderUI(size_t index, std::string value) {
    static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank |
                                            ImGuiInputTextFlags_EnterReturnsTrue;

    const float availWidth = ImGui::GetContentRegionAvailWidth();
    ImGui::SetNextItemWidth(availWidth);

    bool renamed = false;

    if (m_idx != index) {
        if (ImGui::Selectable(value.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                value = "";
                m_renaming = true;
                m_appearing = true;
                m_idx = index;
            }
        }
    } else {
        if (DynamicInputText("##rename", &m_value, edit_flags)) {
            m_renaming = false;

            if (m_value.empty()) {
                m_value = value;
            }

            renamed = true;
            m_idx = -1;
        }

        if (!ImGui::IsItemActive() && !m_appearing) {
            m_renaming = false;
            m_idx = -1;
        }

        if (m_appearing) {
            m_value = value;
            ImGui::SetKeyboardFocusHere();
            m_appearing = false;
        }
    }

    return renamed;
}

void InlineRename::setState(bool state) {
    m_renaming = state;
}

void InlineRename::setValue(const std::string& value) {
    m_initial = value;
    m_value = value;
}

std::string InlineRename::getValue() const {
    return m_value;
}

} // namespace utility
} // namespace aderite