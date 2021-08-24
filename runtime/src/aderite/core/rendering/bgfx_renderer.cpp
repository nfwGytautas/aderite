#include "bgfx_renderer.hpp"

#include <bgfx/bgfx.h>
#include <bx/string.h>

#include "aderite/config.hpp"
#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/core/window/window.hpp"

namespace aderite {
	namespace impl {

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

		bool bgfx_renderer::init(window* wnd) {
			LOG_DEBUG("BGFX renderer");
			
			// Platform data
			bgfx::PlatformData pd;
			pd.nwh = wnd->get_native_handle();

			bgfx::Init bgfxInit;
			bgfxInit.platformData = pd;
			bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a backend
			bgfxInit.resolution.width = wnd->get_size().x;
			bgfxInit.resolution.height = wnd->get_size().y;
			bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
			bgfxInit.callback = &g_cb;

			if (!bgfx::init(bgfxInit)) {
				LOG_ERROR("Failed to initialize BGFX");
			}

			if (!renderer::init(wnd)) {
				return false;
			}

			m_initialized = true;
			engine::get()->renderer_initialized();
			return true;
		}

		void bgfx_renderer::shutdown() {
			renderer::shutdown();
		}

		void bgfx_renderer::set_vsync(bool enabled) {
			LOG_WARN("set_vsync not implemented");
		}

		void bgfx_renderer::begin_frame() {
			if (!ready()) {
				return;
			}

		}

		void bgfx_renderer::end_frame() {
			if (!ready()) {
				return;
			}

			bgfx::frame(false);
		}

		bool bgfx_renderer::ready() {
			return m_initialized;
		}

		void bgfx_renderer::clear() {
			bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x252525FF, 1.0f, 0);
		}

		void bgfx_renderer::output_to_default() {
			
		}

		void bgfx_renderer::reset_output() {
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Set viewport
			glm::vec2 size = m_attached_to->get_size();
			bgfx::setViewRect(0, 0, 0, size.x, size.y);
		}
	}
}