#include "RenderOperation.hpp"

#include "aderite/utility/Log.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

RenderOperation::RenderOperation(EntityProvideOperation* entities, EyeProvideOperation* eye, TargetProvideOperation* target)
	: m_entities(entities), m_eye(eye), m_target(target)
{}

void RenderOperation::execute() {
	LOG_TRACE("Render operation");
}

ADERITE_RENDERING_NAMESPACE_END
