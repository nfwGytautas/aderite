#pragma once

#include "aderite/core/rendering/vao/vao.hpp"

namespace aderite {
	namespace render_backend {
		namespace opengl {

			/**
			 * @brief OpenGL vao implementation
			*/
			class gl_vao : public vao {
			public:
				gl_vao();
				~gl_vao();

				// Inherited via vao
				virtual void bind() override;
				virtual void change_layout(const vao_layout& layout) override;
				virtual void render() override;
			private:
				unsigned int m_vao = 0;

				unsigned int m_positionVbo = 0;
				unsigned int m_ebo = 0;

				unsigned int m_count = 0;
			};

		}
	}
}
