#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/scene/ICamera.hpp"
#include "aderite/rendering/uniform/Forward.hpp"

ADERITE_INTERFACE_NAMESPACE_BEGIN

/**
 * @brief Rendering pass interface
 */
class IPass {
public:
	virtual ~IPass() {};

	/**
	 * @brief Initialize pass
	 * @param um Uniform manager
	 * @return True if pass initialized, false otherwise
	*/
	virtual bool init(rendering::uniform::UniformManager* um) = 0;

	/**
	 * @brief Shutdown pass
	*/
	virtual void shutdown() = 0;

	/**
	 * @brief Make a pass from the specified camera
	 * @param eye Camera to make the pass from
	*/
	virtual void pass(interfaces::ICamera* eye) = 0;

	/**
	 * @brief Function invoked when the window has been resized
	 * @param newWidth New width of the window
	 * @param newHeight New height of the window
	*/
	virtual void onWindowResized(unsigned int newWidth, unsigned int newHeight) {};
};

ADERITE_INTERFACE_NAMESPACE_END
