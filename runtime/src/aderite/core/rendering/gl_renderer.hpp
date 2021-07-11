#pragma once

#include "aderite/core/rendering/renderer.hpp"

namespace aderite {
	namespace render_backend {
		namespace opengl {

			/**
			 * @brief OpenGL renderer implementation
			*/
			class gl_renderer : public renderer {
			public:
				// Inherited via renderer
				virtual bool init(relay_ptr<window> wnd) override;
				virtual void shutdown() override;
				virtual void set_vsync(bool enabled) override;
				
				virtual void begin_frame() override;
				virtual void end_frame() override;
				virtual bool ready() override;
				virtual void clear() override;
				virtual void output_to_default() override;
				virtual void reset_output() override;

			private:
				gl_renderer() {}
				friend class renderer;

			private:
				bool m_initialized = false;
			};
		}
	}
}
