#pragma once

#include <glm/glm.hpp>
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to create an eye provider
*/
class EyeProvideOperation : public OperationBase {
public:
	/**
	 * @brief Returns the computed view matrix
	*/
	virtual const glm::mat4& getViewMatrix() const = 0;

	/**
	 * @brief Returns the computed projection matrix
	*/
	virtual const glm::mat4& getProjectionMatrix() const = 0;

	/**
	 * @brief Returns true if current data is valid
	*/
	virtual bool isValid() const = 0;

	ADERITE_DEBUG_SECTION
	(
		virtual const char* getOperationName() override { return "EyeProvideOperation"; }
	);
};

ADERITE_RENDERING_NAMESPACE_END
