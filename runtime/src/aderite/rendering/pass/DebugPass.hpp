#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/physics/collider/Forward.hpp"
#include "aderite/scene/components/Forward.hpp"
#include "aderite/rendering/pass/IPass.hpp"


ADERITE_PASS_NAMESPACE_BEGIN

/**
 * @brief Rendering pass for debugging
*/
class DebugPass : public interfaces::IPass {
public:

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

	// Inherited via IPass
	virtual bool init(rendering::uniform::UniformManager* um) override;
	virtual void shutdown() override;
	virtual void pass(interfaces::ICamera* eye) override;
	virtual void onWindowResized(unsigned int newWidth, unsigned int newHeight) override;
private:
	const bgfx::ViewId m_debugView = 200;

	// Shaders and uniforms
	bgfx::ProgramHandle m_wireframeShader = BGFX_INVALID_HANDLE;
	uniform::DebugUniform* m_debugUniform = nullptr;

	// Resources
	bgfx::VertexBufferHandle m_cubeVBO = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle m_cubeIBO = BGFX_INVALID_HANDLE;
};

ADERITE_PASS_NAMESPACE_END
