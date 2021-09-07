#include "SkyPass.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "aderite/rendering/uniform/UniformManager.hpp"
#include "aderite/rendering/uniform/SkyboxUniform.hpp"

#include "aderite/rendering/shaders/dx11/skybox.hpp"

ADERITE_PASS_NAMESPACE_BEGIN

bool SkyPass::init(rendering::uniform::UniformManager* um) {
	// Shaders
	bgfx::ShaderHandle vsh = bgfx::createShader(bgfx::makeRef(vs_ibl_skybox_dx11, sizeof(vs_ibl_skybox_dx11)));
	bgfx::ShaderHandle fsh = bgfx::createShader(bgfx::makeRef(fs_ibl_skybox_dx11, sizeof(fs_ibl_skybox_dx11)));
	m_program = bgfx::createProgram(vsh, fsh, true);

	m_uniform = um->getUniform<uniform::SkyboxUniform>();

	return true;
}

void SkyPass::shutdown() {
	bgfx::destroy(m_program);
}

void SkyPass::pass(interfaces::ICamera* eye) {
	// Set persistent matrices
	bgfx::setViewTransform(m_skyView, glm::value_ptr(eye->computeViewMatrix()), glm::value_ptr(eye->computeProjectionMatrix()));

	// Bind state
	bgfx::setViewFrameBuffer(m_skyView, eye->getOutputHandle());


}

void SkyPass::onWindowResized(unsigned int newWidth, unsigned int newHeight) {
	bgfx::setViewRect(m_skyView, 0, 0, newWidth, newHeight);
}

ADERITE_PASS_NAMESPACE_END
