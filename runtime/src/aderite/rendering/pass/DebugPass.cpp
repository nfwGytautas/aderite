#include "DebugPass.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "aderite/Aderite.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/physics/ColliderList.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/collider/BoxCollider.hpp"
#include "aderite/rendering/uniform/UniformManager.hpp"
#include "aderite/rendering/uniform/DebugUniform.hpp"

#include "aderite/rendering/shaders/dx11/wireframe.hpp"

ADERITE_PASS_NAMESPACE_BEGIN

static float s_cubeVertices[] =
{
	-1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, // 0
	 1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, // 1
	-1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // 2
	 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, // 3
	-1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // 4
	 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // 5
	-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, // 6
	 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // 7

	 // Barry centric vertices
	 1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // 8
	 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, // 9
	-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // 10
	 1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // 11
};

static const uint16_t s_cubeIndices[] =
{
	0, 1, 2,
	1, 3, 2,
	4, 6, 5,
	5, 6, 7,
	0, 2, 4,
	4, 2, 6,
	1, 5, 3,
	5, 7, 3,

	// Barry centric indices
	0, 4, 8,
	4, 5, 9,
	2, 3, 10,
	6, 11, 7,
};

void DebugPass::drawCollidersAndTriggers() {

	auto colliderGroup = ::aderite::Engine::getSceneManager()->getCurrentScene()->getEntityRegistry()
		.group<scene::components::CollidersComponent>(
			entt::get<scene::components::TransformComponent>);
	for (auto entity : colliderGroup) {
		auto [colliders, transform] = colliderGroup.get<
			scene::components::CollidersComponent,
			scene::components::TransformComponent>(entity);

		drawCollidersAndTriggers(colliders.Colliders, transform);
	}

}

void DebugPass::drawCollidersAndTriggers(physics::ColliderList* colliders, const scene::components::TransformComponent& transform) {
	for (physics::Collider* collider : *colliders) {
		switch (collider->getType()) {
		case physics::ColliderType::BOX: {
			drawBoxCollider(static_cast<physics::collider::BoxCollider*>(collider), transform);
			break;
		}
		}
	}
}

void DebugPass::drawBoxCollider(physics::collider::BoxCollider* collider, const scene::components::TransformComponent& transform) {
	glm::vec3 extents = collider->getSize();

	scene::components::TransformComponent tempTransform = transform;
	tempTransform.Scale *= extents;

	if (collider->isTrigger()) {
		m_debugUniform->wfColor[0] = 0.0f;
		m_debugUniform->wfColor[1] = 1.0f;
		m_debugUniform->wfColor[2] = 0.0f;
		m_debugUniform->wfOpacity = 1.0f;
		m_debugUniform->update();
	}
	else {
		m_debugUniform->wfColor[0] = 1.0f;
		m_debugUniform->wfColor[1] = 0.0f;
		m_debugUniform->wfColor[2] = 0.0f;
		m_debugUniform->wfOpacity = 1.0f;
		m_debugUniform->update();
	}

	drawCube(tempTransform);
}

void DebugPass::drawCube(const scene::components::TransformComponent& transform) {
	uint64_t state = 0
		| BGFX_STATE_WRITE_RGB
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_CULL_CCW
		| BGFX_STATE_MSAA
		| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
		;

	bgfx::setTransform(glm::value_ptr(scene::components::TransformComponent::computeTransform(transform)));
	bgfx::setState(state);

	bgfx::setVertexBuffer(0, m_cubeVBO);
	bgfx::setIndexBuffer(m_cubeIBO);
	bgfx::submit(m_debugView, m_wireframeShader, 0);
}

void DebugPass::onWindowResized(unsigned int newWidth, unsigned int newHeight) {
	bgfx::setViewRect(m_debugView, 0, 0, newWidth, newHeight);
}

bool DebugPass::init(rendering::uniform::UniformManager* um) {
	// Uniforms
	m_debugUniform = um->getUniform<uniform::DebugUniform>();
	m_debugUniform->wfThickness = 4.0f;
	m_debugUniform->update();

	// Shaders
	bgfx::ShaderHandle vsh = bgfx::createShader(bgfx::makeRef(vs_wf_wireframe_dx11, sizeof(vs_wf_wireframe_dx11)));
	bgfx::ShaderHandle fsh = bgfx::createShader(bgfx::makeRef(fs_wf_wireframe_dx11, sizeof(fs_wf_wireframe_dx11)));
	m_wireframeShader = bgfx::createProgram(vsh, fsh, true);

	// Resources
	bgfx::VertexLayout cubeLayout;
	cubeLayout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color1, 3, bgfx::AttribType::Float)
		.end();

	m_cubeVBO = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), cubeLayout);
	m_cubeIBO = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices)));


	bgfx::setViewClear(m_debugView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x252525FF, 1.0f, 0);

	return true;
}

void DebugPass::shutdown() {
	bgfx::destroy(m_wireframeShader);
	bgfx::destroy(m_cubeVBO);
}

void DebugPass::pass(interfaces::ICamera* eye) {
	// Bind state
	bgfx::setViewFrameBuffer(m_debugView, eye->getOutputHandle());
	bgfx::touch(m_debugView);

	// Set persistent matrices
	bgfx::setViewTransform(
		m_debugView,
		glm::value_ptr(eye->computeViewMatrix()),
		glm::value_ptr(eye->computeProjectionMatrix()));

	// Colliders and triggers
	drawCollidersAndTriggers();
}

ADERITE_PASS_NAMESPACE_END
