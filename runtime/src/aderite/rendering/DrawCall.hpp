#pragma once

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <vector>
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

	// If this value is false the renderer will abort the draw call
	bool Valid = true;

	// Should the draw call be instanced or not, default false
	bool Instanced = false;

	// Vector containing transformations, for instanced rendering
	std::vector<glm::mat4> Transformations;
};

ADERITE_RENDERING_NAMESPACE_END
