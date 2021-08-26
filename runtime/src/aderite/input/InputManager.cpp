#include "InputManager.hpp"

#include "aderite/Config.hpp"
#include "aderite/Aderite.hpp"
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

#else
#error "Unsupported backend for input manager"
#endif

	return true;
}

void InputManager::shutdown() {

}

void InputManager::update() {
#if GLFW_BACKEND == 1
	glfwPollEvents();
#else
#error "Unsupported backend for input manager"
#endif
}

void InputManager::onWindowResized(unsigned int newWidth, unsigned int newHeight) {
	
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

void InputManager::onWindowClosed() {
	::aderite::Engine::get()->requestExit();
}

ADERITE_INPUT_NAMESPACE_END
