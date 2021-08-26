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
	bgfx::VertexBufferHandle VBO = BGFX_INVALID_HANDLE;

	// Index buffer
	bgfx::IndexBufferHandle IBO = BGFX_INVALID_HANDLE;

	// Shader with which to render
	bgfx::ProgramHandle Shader = BGFX_INVALID_HANDLE;

	// Target where to render to
	bgfx::FrameBufferHandle Target = BGFX_INVALID_HANDLE;

	// If this value is false the renderer will abort the draw call
	bool Valid = true;
};

ADERITE_RENDERING_NAMESPACE_END
