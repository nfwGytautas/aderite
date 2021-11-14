#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include "aderite/scripting/Forward.hpp"

#include "aderiteeditor/shared/BaseComponent.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief This is the modal window showed on aderite editor when a script component is being added
 */
class SelectScriptModal : public editor::BaseComponent {
public:
    /**
     * @brief Shows the modal window
     */
    void show();

    /**
     * @brief Render modal
     */
    void render() override;

    /**
     * @brief Returns true if the modal is currently visible, false otherwise
     */
    bool isOpen() const;

    /**
     * @brief Returns the selected behavior wrapper
     */
    const std::string& getSelected() const {
        return m_selected;
    }

    /**
     * @brief Resets the selected behavior
     */
    void reset();

private:
    bool m_visible = false;
    bool m_show = false;

    std::string m_selected = "";
};

} // namespace editor_ui
} // namespace aderite
