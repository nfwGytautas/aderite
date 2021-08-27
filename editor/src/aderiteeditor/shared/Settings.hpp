#pragma once

#include "aderiteeditor/utility/Macros.hpp"
#include "aderite/scene/ICamera.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

/**
 * @brief Various settings for editor
*/
class Settings {
public:
	/**
	 * @brief Pan speed of the editor camera, default 0.01
	*/
	static float EditorCameraSpeed;

	/**
	 * @brief Field of vision of the editor camera, default 90.0f
	*/
	static float EditorCameraFov;
};

ADERITE_EDITOR_SHARED_NAMESPACE_END
