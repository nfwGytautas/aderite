#pragma once

#include "aderite/utility/Macros.hpp"
#include "aderite/input/InputEnums.hpp"

ADERITE_INPUT_NAMESPACE_BEGIN

/**
 * @brief Class responsible for all input actions received from the operating system,
 * this class is closely related to the WindowManager, cause all inputs come through the window
 * and the InputManager is tasked with handling them
*/
class InputManager {
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
	 * @brief Function invoked when a key was pressed on the keyboard
	 * @param key Key that was pressed
	 * @param action Action, for more info look at KeyAction enum
	 * @param modifiers Modifiers applied when the event was submitted
	*/
	void onKeyStateChange(Key key, KeyAction action, KeyModifier modifiers);

	/**
	 * @brief Function invoked when the window has been closed by the user
	*/
	void onWindowClosed();
private:
	InputManager() {}
	friend class Engine;

private:
	/**
	 * @brief True if key is down, false otherwise, slight waste of memory here because, 
	 * Key enum isn't fully sequential and as such there are gaps
	*/
	bool m_keyStates[(size_t)Key::COUNT] = { false };
};

ADERITE_INPUT_NAMESPACE_END
