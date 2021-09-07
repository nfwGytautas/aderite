#include "SkyboxUniform.hpp"

ADERITE_UNIFORM_NAMESPACE_BEGIN

bgfx::UniformType::Enum SkyboxUniform::getType() const {
	return bgfx::UniformType::Enum::Vec4;
}

const char* SkyboxUniform::getName() const {
	return "u_skybox";
}

uint8_t SkyboxUniform::getECount() const {
	return c_Count;
}

const void* SkyboxUniform::getData() const {
	return &Params[0];
}

ADERITE_UNIFORM_NAMESPACE_END
