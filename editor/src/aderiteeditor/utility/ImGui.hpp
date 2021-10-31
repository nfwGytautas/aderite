#pragma once

#include <functional>
#include <string>

#include <glm/glm.hpp>

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace utility {

typedef int ImGuiInputTextFlags;

/**
 * @brief Expands upon ImGui InputText to provide resizing input field
 * @param label Label of the field
 * @param value Value to store in
 * @param flags InputText flags
 * @return Standard InputText return value
 */
bool DynamicInputText(const char* label, std::string* value, ImGuiInputTextFlags flags);

/**
 * @brief Renders a vec3 ui control
 * @param label Label of the control
 * @param values Values
 * @param resetValue Reset value
 * @param columnWidth Width of a single entry field
 * @return True if altered, false otherwise
 */
bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

/**
 * @brief Utility class for creating an inline renaming element
 */
class InlineRename {
public:
    /**
     * @brief Renders ImGui ui, returns true if value was altered
     */
    bool renderUI();

    /**
     * @brief Renders ImGui ui, returns true if value was altered, method used for array operations,
     * for example having an array of renameable objects pass object index.
     * @param index Index of the current render
     * @param value Value of the selectable
     */
    bool renderUI(size_t index, std::string value);

    /**
     * @brief Sets the state of the element if true, then renaming will be initiated,
     * false renaming will be canceled
     */
    void setState(bool state);

    /**
     * @brief Sets the value of the element
     * @param value New value of the element
     */
    void setValue(const std::string& value);

    /**
     * @brief Returns the current value
     */
    std::string getValue() const;

private:
    bool m_renaming = false;
    bool m_appearing = false;
    std::string m_value = "";
    std::string m_initial = "";
    size_t m_idx = -1;
};

} // namespace utility
} // namespace aderite
