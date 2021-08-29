#include "Renderer.hpp"

#include <unordered_map>

#include <glm/gtc/type_ptr.hpp>

#include <bgfx/bgfx.h>
#include <bx/string.h>

#include "aderite/Config.hpp"
#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/bgfx.hpp"
#include "aderite/utility/Utility.hpp"
#include "aderite/window/WindowManager.hpp"
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

	static bgfxCallback g_cb;
}

ADERITE_RENDERING_NAMESPACE_BEGIN

bool Renderer::init() {
	LOG_DEBUG("BGFX Renderer");

	auto windowManager = ::aderite::Engine::getWindowManager();
	glm::i32vec2 size = windowManager->getSize();

	// Platform data
	bgfx::PlatformData pd;
	pd.nwh = windowManager->getNativeHandle();

	bgfx::Init bgfxInit;
	bgfxInit.platformData = pd;
	bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a backend
	bgfxInit.resolution.width = size.x;
	bgfxInit.resolution.height = size.y;
	bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
	bgfxInit.callback = &::impl::g_cb;

	// TODO: Add multi threaded

	if (!bgfx::init(bgfxInit)) {
		LOG_ERROR("Failed to initialize BGFX");
	}

	// Finish any queued operations
	bgfx::frame();

	m_isInitialized = true;
	::aderite::Engine::get()->onRendererInitialized();
	return true;
}

void Renderer::shutdown() {

}

void Renderer::setVsync(bool enabled) {
	ADERITE_UNIMPLEMENTED;
}

void Renderer::clear() {
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x252525FF, 1.0f, 0);
}

void Renderer::resetOutput() {
	// Set SceneView
	glm::i32vec2 size = ::aderite::Engine::getWindowManager()->getSize();
	bgfx::setViewRect(0, 0, 0, size.x, size.y);
}

void Renderer::renderScene(scene::Scene* scene) {
	// TODO: Sorting

	if (!isReady()) {
		return;
	}

	std::unordered_map<size_t, DrawCall> drawCalls;
	size_t drawCallIdx = 0;

	// Resize
	drawCalls.reserve(m_lastRenderDrawCalls);

	// Construct draw calls
	auto group = scene->getEntityRegistry().group<scene::components::TransformComponent>(entt::get<scene::components::MeshRendererComponent>);
	for (auto entity : group) {
		// TODO: Layers
		auto [TransformComponent, mr] = group.get<scene::components::TransformComponent, scene::components::MeshRendererComponent>(entity);

		if (mr.MeshHandle == nullptr || mr.MaterialHandle == nullptr) {
			continue;
		}

		// Check if draw call already exists
		size_t hash = utility::combineHash(mr.MeshHandle->hash(), mr.MaterialHandle->hash());
		DrawCall& dc = drawCalls[hash];

		if (!bgfx::isValid(dc.VBO) || !bgfx::isValid(dc.Shader)) {
			mr.MeshHandle->fillDrawCall(&dc);
			mr.MaterialHandle->fillDrawCall(&dc);
		}

		glm::mat4 tmat = scene::components::TransformComponent::compute_transform(TransformComponent);
		dc.Transformations.push_back(tmat);
	}

	// View 0 clear
	bgfx::touch(0);

	// Skip rendering if there are no draw calls
	if (drawCalls.size() > 0) {
		// Execute draw calls for all cameras
		for (interfaces::ICamera* camera : scene->getCameras()) {
			if (!camera->isEnabled()) {
				// Skip disabled cameras
				continue;
			}

			auto& outputHandle = camera->getOutputHandle();
			if (!bgfx::isValid(outputHandle)) {
				// Skip invalid handles
				continue;
			}

			// Set persistent matrices
			bgfx::setViewTransform(0, glm::value_ptr(camera->computeViewMatrix()), glm::value_ptr(camera->computeProjectionMatrix()));

			// Bind state
			bgfx::setViewFrameBuffer(0, camera->getOutputHandle());
			bgfx::touch(0);

			for (auto& dc : drawCalls) {
				executeDrawCall(dc.second);
			}
		}
	}

	bgfx::discard(BGFX_DISCARD_ALL);

	// Commit
	bgfx::frame(false);

	m_lastRenderDrawCalls = drawCalls.size();
}

bool Renderer::isReady() {
	return m_isInitialized;
}

void Renderer::setResolution(const glm::uvec2& size) {
	bgfx::setViewRect(0, 0, 0, size.x, size.y);
}

void Renderer::displayFrame(bgfx::FrameBufferHandle image) {
	LOG_ERROR("displayFrame() not implemented yet");
}

void Renderer::executeDrawCall(DrawCall& dc) {
	// Check if valid draw call
	if (!dc.Valid) {
		return;
	}

	// Bind buffers
	bgfx::setVertexBuffer(0, dc.VBO);

	if (bgfx::isValid(dc.IBO)) {
		bgfx::setIndexBuffer(dc.IBO);
	}

	if (dc.Instanced) {
		LOG_WARN("Instanced rendering not implemented yet");
	}
	else {
		for (auto& transform : dc.Transformations) {
			bgfx::setTransform(glm::value_ptr(transform));

			// Submit draw call
			uint8_t flags = BGFX_DISCARD_ALL & ~(BGFX_DISCARD_BINDINGS | BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS);
			bgfx::submit(0, dc.Shader, 0, flags);
		}
	}
}

ADERITE_RENDERING_NAMESPACE_END
