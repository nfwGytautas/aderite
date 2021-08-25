#pragma once

#include <functional>
#include <string>
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

typedef int ImGuiInputTextFlags;

/**
 * @brief Expands upon ImGui InputText to provide resizing input field
 * @param label Label of the field
 * @param value Value to store in
 * @param flags InputText flags
 * @return Standard InputText return value
*/
bool DynamicInputText(const char* label, std::string* value, ImGuiInputTextFlags flags);

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
