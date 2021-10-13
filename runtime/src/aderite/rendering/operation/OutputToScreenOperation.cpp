#include "OutputToScreenOperation.hpp"

#include "aderite/utility/Log.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

OutputToScreenOperation::OutputToScreenOperation(TargetProvideOperation* target)
	: m_target(target)
{}

void OutputToScreenOperation::execute() {
	LOG_TRACE("Output to screen operation");
}

ADERITE_RENDERING_NAMESPACE_END
