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
				virtual bool init() override;
				virtual void shutdown() override;
				virtual void set_vsync(bool enabled) override;
				
				virtual void begin_frame() override;
				virtual void end_frame() override;
				virtual void set_output(relay_ptr<fbo> target) override;
				virtual bool ready() override;

			private:
				gl_renderer() {}
				friend class renderer;

			private:
				bool m_initialized = false;
			};
		}
	}
}
