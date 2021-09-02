#include "InputManager.hpp"

#include "aderite/Config.hpp"
#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/window/WindowManager.hpp"

#if GLFW_BACKEND
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

ADERITE_INPUT_NAMESPACE_BEGIN

bool InputManager::init() {
#if GLFW_BACKEND == 1
	GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(::aderite::Engine::getWindowManager()->getImplementationHandle());
	glfwSetWindowUserPointer(glfwWindow, this);

	glfwGetCursorPos(glfwWindow, &m_mousePosition.x, &m_mousePosition.y);

	// Install event callbacks
	
	// Framebuffer size
	glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height)
	{
		InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		im->onWindowResized(width, height);
	});

	// Window close
	glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* window)
	{
		InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		im->onWindowClosed();
	});

	// Key callback
	glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		im->onKeyStateChange(static_cast<Key>(key), static_cast<KeyAction>(action), static_cast<KeyModifier>(mods));
	});

	// Mouse callback
	glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* window, double xpos, double ypos)
	{
		InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		im->onMouseMove(xpos, ypos);
	});

	glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* window, int button, int action, int mods)
	{
		InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		im->onMouseKeyStateChange(static_cast<MouseKey>(button), static_cast<KeyAction>(action), static_cast<KeyModifier>(mods));
	});

	glfwSetScrollCallback(glfwWindow, [](GLFWwindow* window, double xpos, double ypos)
	{
		InputManager* im = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		im->onMouseScrolled(ypos);
});

#else
#error "Unsupported backend for input manager"
#endif

	return true;
}

void InputManager::shutdown() {

}

void InputManager::update() {
	// Reset delta
	m_mouseDelta = {};
	m_mouseScrollDelta = 0.0;

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
		m_keyStates[static_cast<size_t>(key)] = true;
		break;
	}
	default: {
		m_keyStates[static_cast<size_t>(key)] = false;
	}
	}
}

void InputManager::onMouseKeyStateChange(MouseKey key, KeyAction action, KeyModifier modifiers) {
	switch (action) {
	case KeyAction::PRESS:
	case KeyAction::REPEAT: {
		m_mouseKeyStates[static_cast<size_t>(key)] = true;
		break;
	}
	default: {
		m_mouseKeyStates[static_cast<size_t>(key)] = false;
	}
	}
}

void InputManager::onMouseMove(double xPos, double yPos) {
	m_mouseDelta.x = xPos - m_mousePosition.x;
	m_mouseDelta.y = (yPos - m_mousePosition.y) * -1; // Inverted here, so that it would be more intuitive
	m_mousePosition.x = xPos;
	m_mousePosition.y = yPos;
}

void InputManager::onMouseScrolled(double yOffset) {
	m_mouseScrollDelta = yOffset;
}

void InputManager::onWindowClosed() {
	::aderite::Engine::get()->requestExit();
}

bool InputManager::isKeyPressed(Key key) const {
	// TODO: Bound check
	return m_keyStates[static_cast<size_t>(key)];
}

bool InputManager::isMouseKeyPressed(MouseKey key) const {
	// TODO: Bound check
	return m_mouseKeyStates[static_cast<size_t>(key)];
}

glm::dvec2 InputManager::getMousePosition() const {
	return m_mousePosition;
}

glm::dvec2 InputManager::getMouseDelta() const {
	return m_mouseDelta;
}

double InputManager::getScrollDelta() const {
	return m_mouseScrollDelta;
}

ADERITE_INPUT_NAMESPACE_END
