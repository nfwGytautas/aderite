#pragma once

#include "aderite/scene/ICamera.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to create an camera provider
*/
class CameraProvideOperation : public OperationBase {
public:
	virtual ~CameraProvideOperation() {};

	/**
	 * @brief Returns the camera instance
	*/
	interfaces::ICamera* getCamera() const;

	ADERITE_DEBUG_SECTION
	(
		virtual const char* getOperationName() override { return "CameraProvideOperation"; }
	)
protected:
	interfaces::ICamera* p_camera = nullptr;
};

/**
 * @brief Camera provide operation that finds the main camera and returns it
*/
class MainCameraProvideOperation : public CameraProvideOperation {
public:
	/**
	 * @brief Figure out the main camera and set it 
	*/
	virtual void execute() override;

	ADERITE_DEBUG_SECTION
	(
		virtual const char* getOperationName() override { return "MainCameraProvideOperation"; }
	)
};

/**
 * @brief Camera provide operation that always returns the same camera
*/
class FreeCameraProvideOperation : public CameraProvideOperation {
public:
	/**
	 * @brief Set the camera for provider
	*/
	void setCamera(interfaces::ICamera* camera);

	ADERITE_DEBUG_SECTION
	(
		virtual const char* getOperationName() override { return "FreeCameraProvideOperation"; }
	)
};

ADERITE_RENDERING_NAMESPACE_END
