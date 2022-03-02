#pragma once

namespace aderite {
namespace editor {

/**
 * @brief Interface for editor modal windows
 */
class IModal {
public:
    virtual ~IModal() = default;

    /**
     * @brief Render the modal
     */
    virtual void render() = 0;

    /**
     * @brief Returns true if the modal is still valid and should be shown, false otherwise, once this returns false the editor ui will pop
     * it off the stack
     */
    virtual bool stillValid() const = 0;

    /**
     * @brief Close the modal, canceling it and making it invalid
     */
    virtual void close() = 0;

    /**
     * @brief Called once by the UI handler when it's time to display the modal
     */
    virtual void show() = 0;
};

} // namespace editor
} // namespace aderite
