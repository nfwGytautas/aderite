#pragma once

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <vector>
#include "aderite/utility/Macros.hpp"
#include "aderite/scene/components/Forward.hpp"

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

	// Uniform from material type
	bgfx::UniformHandle MaterialUniform = BGFX_INVALID_HANDLE;

	// Samplers
	std::vector<std::pair<bgfx::UniformHandle, bgfx::TextureHandle>> Samplers;

	// Uniform data
	float* UniformData = nullptr;

	// Vector containing transformations, for instanced rendering
	std::vector<scene::components::TransformComponent*> Transformations;

	// Occlusion culled
	bool FullyCulled = false;

	// If true this draw call will be skipped
	bool Skip = false;
};

ADERITE_RENDERING_NAMESPACE_END
