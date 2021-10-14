#pragma once

#include <string>
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

	ADERITE_DEBUG_SECTION
	(
		virtual const char* getOperationName() = 0;
		std::string getDebugName() { return p_debugName; }
		void setDebugName(const std::string& name) { p_debugName = name; }
	)
protected:
	ADERITE_DEBUG_SECTION
	(
		std::string p_debugName = "";
	)
};

ADERITE_RENDERING_NAMESPACE_END
