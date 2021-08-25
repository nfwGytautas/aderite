#pragma once

#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/Forward.hpp"

ADERITE_INTERFACE_NAMESPACE_BEGIN

/**
 * @brief Generic renderable item interface
*/
class IRenderable {
public:
	virtual ~IRenderable() {}

	/**
	 * @brief Fills draw call object information from the object shared::State::
	 * @param dc Draw call instance
	*/
	virtual void fillDrawCall(rendering::DrawCall* dc) = 0;
};

ADERITE_INTERFACE_NAMESPACE_END
