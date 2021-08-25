#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Draw call information
*/
class DrawCall {
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

ADERITE_RENDERING_NAMESPACE_END
