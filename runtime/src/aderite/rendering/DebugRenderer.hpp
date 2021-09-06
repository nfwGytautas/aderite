#pragma once

#include <glm/glm.hpp>
#include <bgfx/bgfx.h>

#include "aderite/utility/Macros.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/physics/collider/Forward.hpp"
#include "aderite/scene/components/Forward.hpp"
#include "aderite/scene/ICamera.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Utility class to provide various debug rendering functionality
*/
class DebugRenderer {
public:
	DebugRenderer();
	~DebugRenderer();

	/**
	 * @brief Renders all debug information
	*/
	void render();

	/**
	 * @brief Sets the camera of the debug renderer
	 * @param camera Camera to render to (usually middleware camera)
	*/
	void setCamera(interfaces::ICamera* camera);

	/**
	 * @brief Renders all colliders and triggers
	*/
	void drawCollidersAndTriggers();

	/**
	 * @brief Renders a collider list
	 * @param colliders Collider list to render
	 * @param Transform Origin transform (passed down to drawBoxCollider)
	*/
	void drawCollidersAndTriggers(physics::ColliderList* colliders, const scene::components::TransformComponent& transform);

	/**
	 * @brief Renders a box collider
	 * @param collider Collider to render
	 * @param Transform Origin transform
	*/
	void drawBoxCollider(physics::collider::BoxCollider* collider, const scene::components::TransformComponent& transform);

	/**
	 * @brief Draws a cube at transform
	*/
	void drawCube(const scene::components::TransformComponent& transform);

	/**
	 * @brief Function invoked when the window has been resized
	 * @param newWidth New width of the window
	 * @param newHeight New height of the window
	*/
	void onWindowResized(unsigned int newWidth, unsigned int newHeight);
private:
	const uint16_t m_viewID = 200;

	// Shaders and uniforms
	bgfx::ProgramHandle m_wireframeShader = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle m_paramsUniform = BGFX_INVALID_HANDLE;
	
	// Resources
	bgfx::VertexBufferHandle m_cubeVBO = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle m_cubeIBO = BGFX_INVALID_HANDLE;

	interfaces::ICamera* m_camera = nullptr;

	union
	{
		struct
		{
			/*0*/struct { float m_camPos[3], m_unused0; };
			/*1*/struct { float m_wfColor[4]; };
			/*2*/struct { float m_unused1, m_wfThickness, m_unused2[2]; };
		};
	
		float m_params[3 * 4];
	};
};

ADERITE_RENDERING_NAMESPACE_END
