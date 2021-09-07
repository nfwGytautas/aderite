#include "DebugUniform.hpp"

ADERITE_UNIFORM_NAMESPACE_BEGIN

bgfx::UniformType::Enum DebugUniform::getType() const {
	return bgfx::UniformType::Enum::Vec4;
}

const char* DebugUniform::getName() const {
	return "u_debug";
}

uint8_t DebugUniform::getECount() const {
	return c_Count;
}

const void* DebugUniform::getData() const {
	return &wfParams[0];
}

ADERITE_UNIFORM_NAMESPACE_END
