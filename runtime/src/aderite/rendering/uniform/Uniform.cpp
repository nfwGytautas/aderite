#include "Uniform.hpp"

#include "aderite/utility/Log.hpp"

ADERITE_UNIFORM_NAMESPACE_BEGIN

Uniform::~Uniform() {
	if (bgfx::isValid(m_handle)) {
		LOG_WARN("Uniform destructor invoked without prior destroy() call");
	}
}

void Uniform::create() {
	m_handle = bgfx::createUniform(getName(), getType(), getECount());
}

void Uniform::destroy() {
	bgfx::destroy(m_handle);
}

void Uniform::update() {
	bgfx::setUniform(m_handle, getData(), getECount());
}

ADERITE_UNIFORM_NAMESPACE_END
