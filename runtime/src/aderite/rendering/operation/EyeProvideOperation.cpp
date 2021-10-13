#include "EyeProvideOperation.hpp"

#include "aderite/utility/Log.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

void EyeProvideOperation::provideFromCamera(CameraProvideOperation* camera) {
	m_camera = camera;
}

void EyeProvideOperation::execute() {
	LOG_TRACE("Eye provide operation");
}

ADERITE_RENDERING_NAMESPACE_END
