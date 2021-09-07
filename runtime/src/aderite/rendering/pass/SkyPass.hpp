#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/pass/IPass.hpp"

ADERITE_PASS_NAMESPACE_BEGIN

/**
 * @brief Rendering pass for skybox
*/
class SkyPass : public interfaces::IPass {
public:

	// Inherited via IPass
	virtual bool init(rendering::uniform::UniformManager* um) override;
	virtual void shutdown() override;
	virtual void pass(interfaces::ICamera* eye) override;
	virtual void onWindowResized(unsigned int newWidth, unsigned int newHeight) override;
private:
	const bgfx::ViewId m_skyView = 5;

	// Shaders and uniforms
	bgfx::ProgramHandle m_program = BGFX_INVALID_HANDLE;
	uniform::SkyboxUniform* m_uniform = nullptr;
};

ADERITE_PASS_NAMESPACE_END
