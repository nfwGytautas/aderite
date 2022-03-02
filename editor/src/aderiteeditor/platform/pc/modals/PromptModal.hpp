#pragma once

#include <functional>
#include <string>
#include <vector>

#include "aderiteeditor/platform/pc/modals/IModal.hpp"

namespace aderite {
namespace editor {

/**
 * @brief A simple prompt modal that displays some message, can be configured to have multiple buttons
 */
class PromptModal : public IModal {
public:
    using ButtonClickFn = std::function<void()>;

    /**
     * @brief Prompt button
     */
    struct PromptButton {
        /**
         * @brief Label of the button to display
         */
        const char* Label;

        /**
         * @brief Callback when button is pressed
         */
        ButtonClickFn Callback;
    };

    /**
     * @brief Creates a prompt modal with the specified prompt
     * @param prompt Prompt to display
     * @param Content that is displayed in the body
     */
    PromptModal(const std::string& prompt, const std::string& content);

    /**
     * @brief Push a prompt button
     * @param button Button information
     */
    void pushButton(PromptButton button);

    // Inherited via IModal
    void render() override;
    bool stillValid() const override;
    void close() override;
    void show() override;

private:
    std::string m_prompt;
    std::string m_content;
    std::vector<PromptButton> m_buttons;

    // Default open
    bool m_open = true;
};

} // namespace editor
} // namespace aderite
