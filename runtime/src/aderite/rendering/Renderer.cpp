#include "Renderer.hpp"

#include <bgfx/bgfx.h>
#include <bx/string.h>

#include "aderite/Config.hpp"
#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/window/WindowManager.hpp"
#include "aderite/rendering/DrawCall.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"

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

	// Initial view rect
	auto windowSize = ::aderite::Engine::getWindowManager()->getSize();
	onWindowResized(windowSize.x, windowSize.y, false);

	// Finish any queued operations
	bgfx::frame();

	m_isInitialized = true;
	::aderite::Engine::get()->onRendererInitialized();
	return true;
}

void Renderer::shutdown() {
	if (m_pipeline) {
		m_pipeline->shutdown();
	}

	bgfx::shutdown();
}

void Renderer::setVsync(bool enabled) {
	ADERITE_UNIMPLEMENTED;
}

void Renderer::onWindowResized(unsigned int newWidth, unsigned int newHeight, bool reset) {
	setResolution(glm::uvec2(newWidth, newHeight));



	if (reset) {
		bgfx::reset(newWidth, newHeight);
	}
}

void Renderer::render() {
	if (!isReady()) {
		return;
	}
	// TODO: Status in editor

	scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
	if (currentScene == nullptr) {
		return;
	}

	if (currentScene->getPipeline() != m_pipeline) {
		this->setPipeline(::aderite::Engine::getSceneManager()->getCurrentScene()->getPipeline());
	}

	if (m_pipeline == nullptr) {
		return;
	}

	m_pipeline->execute();
}

bool Renderer::isReady() {
	return m_isInitialized;
}

void Renderer::setResolution(const glm::uvec2& size) {
	bgfx::setViewRect(0, 0, 0, size.x, size.y);
	bgfx::setViewRect(1, 0, 0, size.x, size.y);
	bgfx::setViewRect(2, 0, 0, size.x, size.y);

	// TODO: Forward to render passes	
}

void Renderer::commit() {
	// Commit
	bgfx::frame(false);
	// TODO: Display stats in editor
	//const bgfx::Stats* stats = bgfx::getStats();
	//LOG_INFO("Commiting {0} draw calls", stats->numDraw);
}

Pipeline* Renderer::getPipeline() const {
	return m_pipeline;
}

void Renderer::setPipeline(Pipeline* pipeline) {
	// Shutdown previous
	if (m_pipeline) {
		m_pipeline->shutdown();
	}

	m_pipeline = pipeline;

	// Initialize new
	if (m_pipeline) {
		m_pipeline->initialize();
	}

	::aderite::Engine::get()->onPipelineChanged(m_pipeline);
}

ADERITE_RENDERING_NAMESPACE_END
