#include "TargetProvideOperation.hpp"

#include <bx/string.h>
#include "aderite/utility/Log.hpp"
#include "aderite/rendering/PipelineState.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

bgfx::TextureFormat::Enum findDepthFormat(uint64_t textureFlags, bool stencil = true) {
	const bgfx::TextureFormat::Enum depthFormats[] = { bgfx::TextureFormat::D16, bgfx::TextureFormat::D32 };
	const bgfx::TextureFormat::Enum depthStencilFormats[] = { bgfx::TextureFormat::D24S8 };

	const bgfx::TextureFormat::Enum* formats = stencil ? depthStencilFormats : depthFormats;
	size_t count = stencil ? BX_COUNTOF(depthStencilFormats) : BX_COUNTOF(depthFormats);

	bgfx::TextureFormat::Enum depthFormat = bgfx::TextureFormat::Count;
	for (size_t i = 0; i < count; i++) {
		if (bgfx::isTextureValid(0, false, 1, formats[i], textureFlags)) {
			depthFormat = formats[i];
			break;
		}
	}

	assert(depthFormat != bgfx::TextureFormat::Enum::Count);
	return depthFormat;
}

TargetProvideOperation::TargetProvideOperation(const TargetParams& params)
	: m_params(params)
{}

TargetProvideOperation::~TargetProvideOperation() {

}

bgfx::FrameBufferHandle TargetProvideOperation::getHandle() const {
	return m_handle;
}

void TargetProvideOperation::initialize() {
	createFramebuffer();
}

void TargetProvideOperation::execute(PipelineState* state) {
	state->pushTarget(m_handle);
}

void TargetProvideOperation::shutdown() {
	bgfx::destroy(m_handle);
}

reflection::Type TargetProvideOperation::getType() const {
	return static_cast<reflection::Type>(reflection::RuntimeTypes::OP_TARGET);
}

bool TargetProvideOperation::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	emitter << YAML::Key << "Blittable" << YAML::Value << m_params.Blittable;
	emitter << YAML::Key << "DepthAttachment" << YAML::Value << m_params.DepthAttachment;
	emitter << YAML::Key << "StencilAttachment" << YAML::Value << m_params.StencilAttachment;
	emitter << YAML::Key << "HDR" << YAML::Value << m_params.HDR;
	return true;
}

bool TargetProvideOperation::deserialize(io::Serializer* serializer, const YAML::Node& data) {
	m_params.Blittable = data["Blittable"].as<bool>();
	m_params.DepthAttachment = data["DepthAttachment"].as<bool>();
	m_params.StencilAttachment = data["StencilAttachment"].as<bool>();
	m_params.HDR = data["HDR"].as<bool>();
	return true;
}

void TargetProvideOperation::createFramebuffer() {
	bgfx::TextureHandle textures[2];
	uint8_t attachments = 0;

	const uint64_t textureFlags =
		(m_params.Blittable ? BGFX_TEXTURE_BLIT_DST : 0) |
		0;

	const uint64_t samplerFlags = 
		BGFX_SAMPLER_MIN_POINT | 
		BGFX_SAMPLER_MAG_POINT | 
		BGFX_SAMPLER_MIP_POINT |
		BGFX_SAMPLER_U_CLAMP | 
		BGFX_SAMPLER_V_CLAMP |
		0;

	// BGRA is often faster (internal GPU format)
	bgfx::TextureFormat::Enum format = m_params.HDR ? bgfx::TextureFormat::RGBA16F : bgfx::TextureFormat::BGRA8;
	assert(bgfx::isTextureValid(0, false, 1, format, BGFX_TEXTURE_RT | samplerFlags));
	textures[attachments++] = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, false, 1, format, BGFX_TEXTURE_RT | samplerFlags | textureFlags);

	if (m_params.DepthAttachment) {
		bgfx::TextureFormat::Enum depthFormat = findDepthFormat(BGFX_TEXTURE_RT_WRITE_ONLY | samplerFlags, m_params.StencilAttachment);
		assert(depthFormat != bgfx::TextureFormat::Enum::Count);
		textures[attachments++] = bgfx::createTexture2D(
			bgfx::BackbufferRatio::Equal, 
			false, 
			1, 
			depthFormat, 
			BGFX_TEXTURE_RT_WRITE_ONLY | samplerFlags | textureFlags);
	}

	m_handle = bgfx::createFrameBuffer(attachments, textures, true);

	if (!bgfx::isValid(m_handle)) {
		LOG_WARN("Failed to create framebuffer");
	}

	ADERITE_DEBUG_SECTION
	(
		bgfx::setName(m_handle, this->getName().c_str());
	)
}

ADERITE_RENDERING_NAMESPACE_END
