#pragma once

#include <glm/glm.hpp>

#include "aderite/input/InputEnums.hpp"

namespace aderite {
class Engine;

namespace input {

/**
 * @brief Class responsible for all input actions received from the operating system,
 * this class is closely related to the WindowManager, cause all inputs come through the window
 * and the InputManager is tasked with handling them
 */
class InputManager final {
public:
    /**
     * @brief Initializes the input manager
     */
    bool init();

    /**
     * @brief Shutdown the input manager
     */
    void shutdown();

    /**
     * @brief Updates input state
     */
    void update();

    /**
     * @brief Function invoked when the window has been resized
     * @param newWidth New width of the window
     * @param newHeight New height of the window
     */
    void onWindowResized(unsigned int newWidth, unsigned int newHeight);

    /**
     * @brief Function invoked when a key was pressed/released on the keyboard
     * @param key Key that was pressed
     * @param action Action, for more info look at KeyAction enum
     * @param modifiers Modifiers applied when the event was submitted
     */
    void onKeyStateChange(Key key, KeyAction action, KeyModifier modifiers);

    /**
     * @brief Function invoked when a mouse key was pressed/released on the keyboard
     * @param key Key that was pressed
     * @param action Action, for more info look at KeyAction enum
     * @param modifiers Modifiers applied when the event was submitted
     */
    void onMouseKeyStateChange(MouseKey key, KeyAction action, KeyModifier modifiers);

    /**
     * @brief Function invoked when a mouse was moved
     * @param xPos New x position of the mouse
     * @param yPos New y position of the mouse
     */
    void onMouseMove(double xPos, double yPos);

    /**
     * @brief Function invoked when a mouse was scrolled
     * @param yOffset New y offset of the scroll wheel
     */
    void onMouseScrolled(double yOffset);

    /**
     * @brief Function invoked when the window has been closed by the user
     */
    void onWindowClosed();

    /**
     * @brief Returns true if the specified key is pressed, false otherwise
     */
    bool isKeyPressed(Key key) const;

    /**
     * @brief Returns true if the specified key was released this frame, false otherwise
     */
    bool wasKeyReleased(Key key) const;

    /**
     * @brief Returns true if the specified mouse key is pressed, false otherwise
     */
    bool isMouseKeyPressed(MouseKey key) const;

    /**
     * @brief Returns the mouse position relative to top-left corner of the screen
     */
    glm::dvec2 getMousePosition() const;

    /**
     * @brief Returns the mouse position change since last frame
     */
    glm::dvec2 getMouseDelta() const;

    /**
     * @brief Returns the mouse scroll change since last frame
     */
    double getScrollDelta() const;

private:
    InputManager() {}
    friend Engine;

private:
    struct InputManagerState {
        /**
         * @brief True if key is down, false otherwise, slight waste of memory here because,
         * Key enum isn't fully sequential and as such there are gaps
         */
        bool KeyStates[(size_t)Key::COUNT] = {false};

        /**
         * @brief True if key is down, false otherwise
         */
        bool MouseKeyStates[(size_t)MouseKey::COUNT] = {false};

        /**
         * @brief Mouse position
         */
        glm::dvec2 MousePosition;

        /**
         * @brief Mouse scroll current position
         */
        double MouseScroll = 0.0;
    };

    InputManagerState m_currentFrameState;
    InputManagerState m_previousFrameState;
};

} // namespace input
} // namespace aderite
