#pragma once

#include "aderite/utility/macros.hpp"
#include "aderite/rendering/Forward.hpp"

ADERITE_INTERFACE_NAMESPACE_BEGIN

/**
 * @brief Generic renderable item interface
*/
class IRenderable {
public:
	virtual ~IRenderable() {}

	/**
	 * @brief Fills draw call object information from the object state
	 * @param dc Draw call instance
	*/
	virtual void fill_draw_call(rendering::DrawCall* dc) = 0;
};

ADERITE_INTERFACE_NAMESPACE_END
