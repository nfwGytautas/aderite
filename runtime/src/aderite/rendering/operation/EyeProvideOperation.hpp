#pragma once

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

	// Inherited via OperationBase
	virtual void execute() override;
private:
	CameraProvideOperation* m_camera = nullptr;
};

ADERITE_RENDERING_NAMESPACE_END
