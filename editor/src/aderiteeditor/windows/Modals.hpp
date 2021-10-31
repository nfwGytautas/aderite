#pragma once

#include <functional>
#include <string>

#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief Text input modal with two buttons
 */
class TextInputModal : public editor::BaseComponent {
public:
    using ConfirmFn = std::function<void(const std::string&)>;
    using CancelFn = std::function<void()>;

    /**
     * @brief Input options
     */
    struct Options {
        bool AllowEmpty;

        Options() {
            AllowEmpty = false;
        }
    };

public:
    TextInputModal(const std::string& title = "Title", const std::string& text = "Text input modal", const Options& options = Options());

    /**
     * @brief Show modal
     */
    void show();

    /**
     * @brief Set the modal text
     */
    void setText(const std::string& text);

    /**
     * @brief Set the modal title
     */
    void setTitle(const std::string& text);

    /**
     * @brief Sets the confirm function to be invoked
     */
    void setConfirmAction(ConfirmFn fn);

    /**
     * @brief Sets the cancel function to be invoked
     */
    void setCancelAction(CancelFn fn);

    /**
     * @brief Set the confirm button text
     */
    void setConfirmButtonText(const std::string& text);

    // Inherited via BaseComponent
    virtual void render() override;

private:
    std::string m_text;
    std::string m_title;
    std::string m_confirmText = "Confirm";
    std::string m_value = "";
    bool m_show = false;
    Options m_options = {};

    ConfirmFn m_confirm;
    CancelFn m_cancel;
};

} // namespace editor_ui
} // namespace aderite
