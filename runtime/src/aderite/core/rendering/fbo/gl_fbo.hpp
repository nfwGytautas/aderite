#pragma once

#include "aderite/core/rendering/fbo/fbo.hpp"

namespace aderite {
	namespace render_backend {
		namespace opengl {

			/**
			 * @brief OpenGL fbo implementation
			*/
			class gl_fbo : public fbo {
			public:
				~gl_fbo();

				// Inherited via fbo
				virtual void bind() override;

				unsigned int get_ta() {
					return m_tcolor;
				}

			private:
				gl_fbo(const fbo_create_args& args);
				friend class fbo;

				unsigned int m_fbo;

				// Attachments

				// Color
				unsigned int m_tcolor;

				// Depth and stencil
				unsigned int m_dsrbo;
			};
		}
	}
}
