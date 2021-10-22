#pragma once

#include "aderite/Handles.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"

namespace aderite {
namespace reflection {

/**
 * @brief Reflectable object used to make an interface for reflection system
*/
class Reflectable {
public:
	virtual ~Reflectable() {}

	/**
	 * @brief Returns the type of the reflectable
	*/
	virtual Type getType() const = 0;
};

}
}
