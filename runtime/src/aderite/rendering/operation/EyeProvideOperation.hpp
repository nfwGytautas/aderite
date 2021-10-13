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
	 * @brief Provide eye from camera
	 * @param camera Camera to provide from
	*/
	void provideFromCamera(CameraProvideOperation* camera);

	/**
	 * @brief Returns the computed view matrix
	*/
	const glm::mat4& getViewMatrix() const;

	/**
	 * @brief Returns the computed projection matrix
	*/
	const glm::mat4& getProjectionMatrix() const;

	/**
	 * @brief Returns true if current data is valid
	*/
	bool isValid() const;

	// Inherited via OperationBase
	virtual void execute() override;
private:
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;

	CameraProvideOperation* m_camera = nullptr;
};

ADERITE_RENDERING_NAMESPACE_END
