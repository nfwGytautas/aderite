#pragma once

#include <glm/glm.hpp>
#include <bgfx/bgfx.h>
#include "aderite/utility/macros.hpp"

ADERITE_INTERFACE_NAMESPACE_BEGIN

/**
 * @brief Camera interface
*/
class ICamera {
public:
	virtual ~ICamera() {};

	/**
	 * @brief Returns the view matrix for this camera
	*/
	virtual glm::mat4 computeViewMatrix() = 0;

	/**
	 * @brief Returns the projection matrix for this camera
	*/
	virtual glm::mat4 computeProjectionMatrix() = 0;

	/**
	 * @brief Function used to update camera parameters
	 * @param delta Time between frames
	*/
	virtual void update(float delta) = 0;

	/**
	 * @brief True if the camera is enabled (renderer should render to it), false otherwise
	*/
	virtual bool isEnabled() = 0;
};

ADERITE_INTERFACE_NAMESPACE_END
