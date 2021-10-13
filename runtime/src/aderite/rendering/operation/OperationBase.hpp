#pragma once

#include "aderite/utility/Macros.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Rendering operation base class
*/
class OperationBase {
public:
	virtual ~OperationBase() {};

	/**
	 * @brief Execute the operation
	*/
	virtual void execute() = 0;
};

ADERITE_RENDERING_NAMESPACE_END
