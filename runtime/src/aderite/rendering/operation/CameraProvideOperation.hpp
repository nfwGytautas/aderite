#pragma once

#include <glm/glm.hpp>
#include "aderite/scene/components/Components.hpp"
#include "aderite/rendering/operation/EyeProvideOperation.hpp"
#include "aderite/rendering/operation/Forward.hpp"

namespace aderite {
namespace rendering {


/**
 * @brief Operation used to create an entity camera provider
*/
class CameraProvideOperation : public EyeProvideOperation {
public:
	virtual ~CameraProvideOperation() {};

	/**
	 * @brief Figure out the main camera and set it
	*/
	virtual void execute() override;

	// Inherited via CameraProvideOperation
	// Inherited via EyeProvideOperation
	virtual const glm::mat4& getViewMatrix() const override;
	virtual const glm::mat4& getProjectionMatrix() const override;
	virtual bool isValid() const override;

	ADERITE_DEBUG_SECTION
	(
		virtual const char* getOperationName() override { return "CameraProvideOperation"; }
	);
private:
	bool m_valid = false;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projMatrix;
};

}
}
