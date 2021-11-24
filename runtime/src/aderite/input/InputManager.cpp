#include "InputManager.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/Config.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"
#include "aderite/window/WindowManager.hpp"

#if GLFW_BACKEND
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

namespace aderite {
namespace input {

bool InputManager::init() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[IO] Initializing input manager");

#if GLFW_BACKEND == 1
    LOG_DEBUG("[IO] GLFW backend IO");

    GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(::aderite::Engine::getWindowManager()->getImplementationHandle());
    glfwSetWindowUserPointer(glfwWindow, this);

    glfwGetCursorPos(glfwWindow, &m_currentFrameState.MousePosition.x, &m_currentFrameState.MousePosition.y);

    // Install event callbacks
    LOG_TRACE("[IO] Setting input callbacks for GLFW");

    // Framebuffer size
    glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height) {
        InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
        im->onWindowResized(width, height);
    });

    // Window close
    glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* window) {
        InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
        im->onWindowClosed();
    });

    // Key callback
    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
        im->onKeyStateChange(static_cast<Key>(key), static_cast<KeyAction>(action), static_cast<KeyModifier>(mods));
    });

    // Mouse callback
    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* window, double xpos, double ypos) {
        InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
        im->onMouseMove(xpos, ypos);
    });

    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* window, int button, int action, int mods) {
        InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
        im->onMouseKeyStateChange(static_cast<MouseKey>(button), static_cast<KeyAction>(action), static_cast<KeyModifier>(mods));
    });

    glfwSetScrollCallback(glfwWindow, [](GLFWwindow* window, double xpos, double ypos) {
        InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
        im->onMouseScrolled(ypos);
    });

    LOG_INFO("[IO] Callbacks installed");

#else
#error "Unsupported backend for input manager"
#endif

    LOG_INFO("[IO] Input manager initialized");
    return true;
}

void InputManager::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[IO] Shutting down input manager");
    LOG_INFO("[IO] Input manager shutdown");
}

void InputManager::update() {
    // Copy state
    m_previousFrameState = m_currentFrameState;
    m_currentFrameState.MouseScroll = 0;

#if GLFW_BACKEND == 1
    glfwPollEvents();
#else
#error "Unsupported backend for input manager"
#endif
}

void InputManager::onWindowResized(unsigned int newWidth, unsigned int newHeight) {
    ::aderite::Engine::get()->onWindowResized(newWidth, newHeight);
}

void InputManager::onKeyStateChange(Key key, KeyAction action, KeyModifier modifiers) {
    switch (action) {
    case KeyAction::PRESS:
    case KeyAction::REPEAT: {
        m_currentFrameState.KeyStates[static_cast<size_t>(key)] = true;
        break;
    }
    default: {
        m_currentFrameState.KeyStates[static_cast<size_t>(key)] = false;
    }
    }
}

void InputManager::onMouseKeyStateChange(MouseKey key, KeyAction action, KeyModifier modifiers) {
    switch (action) {
    case KeyAction::PRESS:
    case KeyAction::REPEAT: {
        m_currentFrameState.MouseKeyStates[static_cast<size_t>(key)] = true;
        break;
    }
    default: {
        m_currentFrameState.MouseKeyStates[static_cast<size_t>(key)] = false;
    }
    }
}

void InputManager::onMouseMove(double xPos, double yPos) {
    m_currentFrameState.MousePosition.x = xPos;
    m_currentFrameState.MousePosition.y = yPos;
}

void InputManager::onMouseScrolled(double yOffset) {
    m_currentFrameState.MouseScroll = yOffset;
}

void InputManager::onWindowClosed() {
    ::aderite::Engine::get()->requestExit();
}

bool InputManager::isKeyPressed(Key key) const {
    // TODO: Bound check
    return m_currentFrameState.KeyStates[static_cast<size_t>(key)];
}

bool InputManager::wasKeyReleased(Key key) const {
    return m_previousFrameState.KeyStates[static_cast<size_t>(key)] == true && m_currentFrameState.KeyStates[static_cast<size_t>(key)] == false;
}

bool InputManager::isMouseKeyPressed(MouseKey key) const {
    // TODO: Bound check
    return m_currentFrameState.MouseKeyStates[static_cast<size_t>(key)];
}

glm::dvec2 InputManager::getMousePosition() const {
    return m_currentFrameState.MousePosition;
}

glm::dvec2 InputManager::getMouseDelta() const {
    glm::dvec2 result = m_currentFrameState.MousePosition;
    result.x = result.x - m_previousFrameState.MousePosition.x;
    result.y = (result.y - m_previousFrameState.MousePosition.y) * -1; // Inverted here, so that it would be more intuitive
    return result;
}

double InputManager::getScrollDelta() const {
    return m_currentFrameState.MouseScroll;
}

} // namespace input
} // namespace aderite
