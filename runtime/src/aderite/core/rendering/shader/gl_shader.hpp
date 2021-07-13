#pragma once

#include "aderite/core/rendering/shader/shader.hpp"

namespace aderite {
	namespace render_backend {
		namespace opengl {

			/**
			 * @brief OpenGL shader implementation
			*/
			class gl_shader : public shader {
			public:
				~gl_shader();

				virtual void bind() override;

			private:
				gl_shader(const shader_create_args& args);
				friend class shader;

				unsigned int m_shader = 0;
			};

		}
	}
}
