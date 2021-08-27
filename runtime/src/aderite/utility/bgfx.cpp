#include "bgfx.hpp"

#include <bx/string.h>

#include "aderite/utility/Log.hpp"

ADERITE_UTILITY_NAMESPACE_BEGIN 

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

bgfx::FrameBufferHandle createFrameBuffer(bool hdr, bool depth) {
	bgfx::TextureHandle textures[2];
	uint8_t attachments = 0;

	const uint64_t samplerFlags = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT |
		BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

	// BGRA is often faster (internal GPU format)
	bgfx::TextureFormat::Enum format = hdr ? bgfx::TextureFormat::RGBA16F : bgfx::TextureFormat::BGRA8;
	assert(bgfx::isTextureValid(0, false, 1, format, BGFX_TEXTURE_RT | samplerFlags));
	textures[attachments++] = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, false, 1, format, BGFX_TEXTURE_RT | samplerFlags);

	if (depth) {
		bgfx::TextureFormat::Enum depthFormat = findDepthFormat(BGFX_TEXTURE_RT_WRITE_ONLY | samplerFlags);
		assert(depthFormat != bgfx::TextureFormat::Enum::Count);
		textures[attachments++] = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, false, 1, depthFormat, BGFX_TEXTURE_RT_WRITE_ONLY | samplerFlags);
	}

	bgfx::FrameBufferHandle fb = bgfx::createFrameBuffer(attachments, textures, true);

	if (!bgfx::isValid(fb)) {
		LOG_WARN("Failed to create framebuffer");
	}

	return fb;
}

ADERITE_UTILITY_NAMESPACE_END
