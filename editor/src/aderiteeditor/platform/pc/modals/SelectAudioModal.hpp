#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include "aderite/scripting/Forward.hpp"

#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief This is the modal window showed on aderite editor when a audio clip is being selected
 */
class SelectAudioModal : public UIComponent {
public:
    /**
     * @brief Shows the modal window
     */
    void show();

    /**
     * @brief Returns true if the modal is currently visible, false otherwise
     */
    bool isOpen() const;

    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;

    /**
     * @brief Returns the selected event
     */
    std::string getSelectedEvent() const;

    /**
     * @brief Resets the selected event
     */
    void reset();

private:
    bool m_visible = false;
    bool m_show = false;

    std::string m_selected;
};

} // namespace editor
} // namespace aderite
