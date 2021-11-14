#pragma once

#include <string>

#include <glm/glm.hpp>

namespace aderite {
class Engine;

namespace window {
/**
 * @brief Window manager of aderite engine, this allows to create and manipulate a SINGLE output window.
 */
class WindowManager final {
public:
    /**
     * @brief Initializes the window manager
     */
    bool init();

    /**
     * @brief Shutdown the window manager
     */
    void shutdown();

    /**
     * @brief Set the width and height of the display to the one specified
     * @param width New width of the window
     * @param height New height of the window
     */
    void setSize(unsigned int width, unsigned int height) const;

    /**
     * @brief Returns the size of the window
     * @return The size of the window
     */
    glm::i32vec2 getSize() const;

    /**
     * @brief Returns true if the window is closed, false otherwise
     */
    bool isClosed() const;

    /**
     * @brief Set the title of the display to the one specified
     * @param title New title of the display
     */
    void setTitle(const std::string& title) const;

    /**
     * @brief Returns the native system handle of the display, for example on windows this will return HWND
     */
    void* getNativeHandle() const;

    /**
     * @brief Returns the implementation framework of the display, for desktop applications this will be a GLFWwindow
     */
    void* getImplementationHandle() const;

private:
    WindowManager() {}
    friend Engine;

    // Following functions are implemented depending on the platform
    bool backendInit() const;
    bool backendCreateWindow() const;
    void backendShutdown() const;

private:
    class PlatformImpl;
    PlatformImpl* m_impl = nullptr;
};

} // namespace window
} // namespace aderite
