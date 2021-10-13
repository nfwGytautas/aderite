#include "DebugOperation.hpp"

#include "aderite/utility/Log.hpp"

#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

DebugOperation::DebugOperation() {
	m_camera = new FreeCameraProvideOperation();
	m_target = new TargetProvideOperation();
}

FreeCameraProvideOperation* DebugOperation::getCameraProvider() const {
	return m_camera;
}

TargetProvideOperation* DebugOperation::getTargetProvider() const {
	return m_target;
}

void DebugOperation::initialize() {

}

void DebugOperation::execute() {

}

void DebugOperation::shutdown() {

}

ADERITE_RENDERING_NAMESPACE_END
