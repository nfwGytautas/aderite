#pragma once

#include <bgfx/bgfx.h>

namespace aderite {
	namespace rendering {

		/**
		 * @brief Draw call information
		*/
		class draw_call {
		public:
			// Vertex buffer that contains data to be rendered
			bgfx::VertexBufferHandle VBO;

			// Index buffer
			bgfx::IndexBufferHandle IBO;

			// Shader with which to render
			bgfx::ProgramHandle Shader;

			// Target where to render to
			bgfx::FrameBufferHandle Target;
		};

	}
}