#pragma once

#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to provide a debug rendering
*/
class DebugOperation : public OperationBase {
public:
	DebugOperation();

	/**
	 * @brief Returns the camera provider operation of the this debug operation
	*/
	FreeCameraProvideOperation* getCameraProvider() const;

	/**
	 * @brief Returns the target provider operation of the this debug operation
	*/
	TargetProvideOperation* getTargetProvider() const;

	// Inherited via OperationBase
	virtual void initialize() override;
	virtual void execute() override;
	virtual void shutdown() override;
private:
	FreeCameraProvideOperation* m_camera = nullptr;
	TargetProvideOperation* m_target = nullptr;
};

ADERITE_RENDERING_NAMESPACE_END
