#pragma once

#include <string>

namespace aderite {
namespace editor {

/**
 * @brief Base class for editor components, that has some commonly used functionality aswell as some getters and setters for projects
 */
class UIComponent {
public:
    virtual ~UIComponent();

    /**
     * @brief Initialize the UI component
     * @return True if component initialized without errors, false otherwise
     */
    virtual bool init() = 0;

    /**
     * @brief Shutdown the UI component
     */
    virtual void shutdown() = 0;

    /**
     * @brief Renders the component
     */
    virtual void render() = 0;

protected:
    /**
     * @brief Utility function that can be used in canRender to make the component render only when a valid project is loaded
     */
    bool isProjectLoaded() const;

    /**
     * @brief Calls ImGui functions to set the properties for the next begin call which will usually be this component
     */
    void applyWindowProperties();

    /**
     * @brief Render icon
     * @param name Name of the icon
     * @param width Width of the icon
     * @param height Height of the icon
     */
    void renderIcon(const std::string& name, size_t width, size_t height);

    /**
     * @brief Renders a clickable icon
     * @param name Name of the icon
     * @param width Width of the icon
     * @param height Height of the icon
     * @return True if clicked, false otherwise
    */
    bool renderIconButton(const std::string& name, size_t width, size_t height);

    /**
     * @brief Sets the imgui cursor to the center of the current window
     * @param elementWidth Element width
     * @param elementHeight Element height
    */
    void setCursorToCenter(const size_t elementWidth, const size_t elementHeight);

private:
};

} // namespace editor
} // namespace aderite
