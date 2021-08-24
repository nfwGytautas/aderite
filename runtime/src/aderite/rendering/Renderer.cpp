#include "Renderer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <bgfx/bgfx.h>
#include <bx/string.h>

// TODO: Move to glm
// TEMPORARY
#include <bx/math.h>

#include "aderite/config.hpp"
#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/window/Window.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/ShaderAsset.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/rendering/DrawCall.hpp"

namespace impl {

	/**
	 * @brief bgfx logging callback
	*/
	class bgfxCallback : public bgfx::CallbackI
	{
	public:
		virtual ~bgfxCallback() { }
		virtual void fatal(const char* filePath, uint16_t line, bgfx::Fatal::Enum code, const char* str) override {
			if (code != bgfx::Fatal::DebugCheck) {
				LOG_FATAL("{0}", str);
			}
		}

		virtual void traceVargs(const char* filePath, uint16_t line, const char* format, va_list args) override {
			char buffer[1024];
			int32_t written = bx::vsnprintf(buffer, BX_COUNTOF(buffer), format, args);
			if (written > 0 && written < BX_COUNTOF(buffer))
			{
				// bgfx sends lines with newlines, spdlog adds another
				if (buffer[written - 1] == '\n')
					buffer[written - 1] = '\0';
				LOG_TRACE(buffer);
			}
		}

		virtual void profilerBegin(const char*, uint32_t, const char*, uint16_t) override { }
		virtual void profilerBeginLiteral(const char*, uint32_t, const char*, uint16_t) override { }
		virtual void profilerEnd() override { }
		virtual uint32_t cacheReadSize(uint64_t) override {
			return 0;
		}
		virtual bool cacheRead(uint64_t, void*, uint32_t) override {
			return false;
		}
		virtual void cacheWrite(uint64_t, const void*, uint32_t) override { }
		virtual void captureBegin(uint32_t, uint32_t, uint32_t, bgfx::TextureFormat::Enum, bool) override { }
		virtual void captureEnd() override { }
		virtual void captureFrame(const void*, uint32_t) override { }
		virtual void screenShot(const char*, uint32_t, uint32_t, uint32_t, const void*, uint32_t, bool yflip) override { }
	};

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

	static bgfxCallback g_cb;
}

ADERITE_RENDERING_NAMESPACE_BEGIN

Renderer* Renderer::create_instance() {
	LOG_TRACE("Creating Renderer instance");
	return new Renderer();
}

bool Renderer::init(window::Window* wnd) {
	LOG_DEBUG("BGFX Renderer");

	// Platform data
	bgfx::PlatformData pd;
	pd.nwh = wnd->get_native_handle();

	bgfx::Init bgfxInit;
	bgfxInit.platformData = pd;
	bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a backend
	bgfxInit.resolution.width = wnd->get_size().x;
	bgfxInit.resolution.height = wnd->get_size().y;
	bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
	bgfxInit.callback = &::impl::g_cb;

	if (!bgfx::init(bgfxInit)) {
		LOG_ERROR("Failed to initialize BGFX");
	}

	// Setup default render target
	m_attached_to = wnd;

	// Create default output framebuffer
	m_output = ::impl::createFrameBuffer();
	bgfx::setName(m_output, "Renderer output framebuffer (pre-postprocessing)");

	// Finish any queued operations
	bgfx::frame();

	m_isInitialized = true;
	engine::get()->renderer_initialized();
	return true;
}

void Renderer::shutdown() {

}

void Renderer::set_vsync(bool enabled) {
	LOG_WARN("set_vsync not implemented");
}

void Renderer::clear() {
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x252525FF, 1.0f, 0);
}

void Renderer::reset_output() {
	// Set viewport
	glm::vec2 size = m_attached_to->get_size();
	bgfx::setViewRect(0, 0, 0, size.x, size.y);
}

void Renderer::render() {
	if (!isReady()) {
		return;
	}

	DrawCall dc;

	// Default target is Renderer viewport
	dc.Target = m_output;

	// TODO: Sorting, batching, output target
	// TODO: Camera, position, perspective matrix

	// Bind state
	bgfx::setViewFrameBuffer(0, dc.Target);
	bgfx::touch(0);

	// Render entities
	scene::scene* scene = engine::get_scene_manager()->current_scene();
	auto group = scene->get_entity_registry().group<scene::components::TransformComponent>(entt::get<scene::components::MeshRendererComponent>);
	for (auto entity : group) {
		// TODO: Layers
		auto [TransformComponent, mr] = group.get<scene::components::TransformComponent, scene::components::MeshRendererComponent>(entity);

		if (mr.MeshHandle == nullptr || mr.MaterialHandle == nullptr) {
			continue;
		}

		mr.MeshHandle->fill_draw_call(&dc);
		mr.MaterialHandle->fill_draw_call(&dc);

		glm::mat4 tmat = scene::components::TransformComponent::compute_transform(TransformComponent);

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

bool Renderer::isReady() {
	return m_isInitialized;
}

void Renderer::begin_frame() {
	if (!isReady()) {
		return;
	}

}

void Renderer::end_frame() {
	if (!isReady()) {
		return;
	}

	bgfx::frame(false);
}

bgfx::FrameBufferHandle Renderer::get_output() {
	return m_output;
}

void Renderer::display_frame() {
	LOG_ERROR("display_frame() not implemented yet");
}

ADERITE_RENDERING_NAMESPACE_END
