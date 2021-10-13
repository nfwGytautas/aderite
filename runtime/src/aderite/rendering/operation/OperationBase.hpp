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
	 * @brief Initialize the operation
	*/
	virtual void initialize() {};

	/**
	 * @brief Execute the operation
	*/
	virtual void execute() {};

	/**
	 * @brief Free all resources
	*/
	virtual void shutdown() {};
};

ADERITE_RENDERING_NAMESPACE_END
