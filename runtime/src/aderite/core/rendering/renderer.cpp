#include "renderer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "aderite/config.hpp"
#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/core/window/window.hpp"
#include "aderite/core/assets/object/mesh_asset.hpp"
#include "aderite/core/assets/object/material_asset.hpp"
#include "aderite/core/assets/object/shader_asset.hpp"
#include "aderite/core/scene/scene_manager.hpp"
#include "aderite/core/scene/components.hpp"
#include "aderite/core/scene/scene.hpp"
#include "aderite/core/rendering/bgfx_renderer.hpp"
#include "aderite/core/rendering/draw_call.hpp"

// TEMPORARY
#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace aderite {

	bgfx::TextureFormat::Enum findDepthFormat(uint64_t textureFlags, bool stencil = false) {
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

	bgfx::FrameBufferHandle createFrameBuffer(bool hdr = true, bool depth = true) {
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
	
	renderer* renderer::create_instance() {
		LOG_TRACE("Creating renderer instance");
		return new impl::bgfx_renderer();
	}

	bool renderer::init(window* wnd) {
		// Setup default render target
		m_attached_to = wnd;

		// Create default output framebuffer
		m_output = createFrameBuffer();
		bgfx::setName(m_output, "Renderer output framebuffer (pre-postprocessing)");

		// Finish any queued operations
		bgfx::frame();

		return true;
	}

	void renderer::shutdown() {

	}

	void renderer::render() {
		if (!ready()) {
			return;
		}

		rendering::draw_call dc;

		// Default target is renderer viewport
		dc.Target = m_output;

		// TODO: Sorting, batching, output target
		// TODO: Camera, position, perspective matrix

		// Bind state
		bgfx::setViewFrameBuffer(0, dc.Target);
		bgfx::touch(0);

		// Render entities
		scene::scene* scene = engine::get_scene_manager()->current_scene();
		auto group = scene->get_entity_registry().group<scene::components::transform>(entt::get<scene::components::mesh_renderer>);
		for (auto entity : group) {
			// TODO: Layers
			auto [transform, mr] = group.get<scene::components::transform, scene::components::mesh_renderer>(entity);

			if (mr.MeshHandle == nullptr || mr.MaterialHandle == nullptr) {
				continue;
			}

			mr.MeshHandle->fill_draw_call(&dc);
			mr.MaterialHandle->fill_draw_call(&dc);

			glm::mat4 tmat = scene::components::transform::compute_transform(transform);

			// Matrices
			const bx::Vec3 at = { 0.0f, 0.0f,  0.0f };
			const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
			float view[16];
			bx::mtxLookAt(view, eye, at);
			float proj[16];
			glm::vec2 wsize = m_attached_to->get_size();
			bx::mtxProj(proj, 60.0f, float(wsize.x) / float(wsize.y), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
			bgfx::setViewTransform(0, view, proj);
			float model[16];
			bgfx::setTransform(glm::value_ptr(tmat));

			// Bind buffers
			bgfx::setVertexBuffer(0, dc.VBO);

			if (bgfx::isValid(dc.IBO)) {
				bgfx::setIndexBuffer(dc.IBO);
			}

			// Default render state
			//bgfx::setState(BGFX_STATE_DEFAULT);

			// Submit draw call
			bgfx::submit(0, dc.Shader);
		}

		//bgfx::discard();

		// Reset to default
		//bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
	}

	bgfx::FrameBufferHandle renderer::get_output() {
		return m_output;
	}

	void renderer::display_frame() {
		LOG_ERROR("display_frame() not implemented yet");
	}

}