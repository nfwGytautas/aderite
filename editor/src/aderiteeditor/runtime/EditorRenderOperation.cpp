#include "EditorRenderOperation.hpp"

#include <filesystem>
#include <bgfx/bgfx.h>
#include <glm/gtc/type_ptr.hpp>
#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/io/RuntimeSerializables.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/ColliderList.hpp"
#include "aderite/physics/collider/BoxCollider.hpp"
#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderiteeditor/runtime/data/Data.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

EditorRenderOperation::EditorRenderOperation() {
	p_debugName = "EditorRenderHook";
}

EditorRenderOperation::~EditorRenderOperation() {

}

void EditorRenderOperation::initialize() {
	ADERITE_DEBUG_SECTION
	(
		bgfx::setViewName(c_ViewId, p_debugName.c_str());
	)

	// Load assets
	loadMeshes();
	loadShaders();

	// Create data
	m_editorUniform = bgfx::createUniform("u_editor", bgfx::UniformType::Vec4, 2);
	wfThickness = 4.0f;
	updateUniform();

	// Since the editor should be hooked into the entity rendering, only clear the depth
	bgfx::setViewClear(c_ViewId, BGFX_CLEAR_DEPTH, 0x252525FF, 1.0f, 0);
	bgfx::setViewRect(c_ViewId, 0, 0, 1280, 720);
}

void EditorRenderOperation::execute() {
	bgfx::discard(BGFX_DISCARD_ALL);
	renderPhysicsObjects();
	bgfx::discard(BGFX_DISCARD_ALL);
}

void EditorRenderOperation::shutdown() {
	bgfx::destroy(m_editorUniform);
	bgfx::destroy(m_wireframeShader);
	bgfx::destroy(m_bcCube);
	bgfx::destroy(m_bcCubeIbo);
}

void EditorRenderOperation::updateUniform() {
	bgfx::setUniform(m_editorUniform, &EditorParameters[0], 2);
}

void EditorRenderOperation::renderPhysicsObjects() {
	// Bind state
	bgfx::setViewFrameBuffer(c_ViewId, editor::State::DebugRenderHandle);
	bgfx::touch(c_ViewId);

	// Set persistent matrices
	bgfx::setViewTransform(
		c_ViewId,
		glm::value_ptr(editor::State::EditorCamera->getViewMatrix()),
		glm::value_ptr(editor::State::EditorCamera->getProjectionMatrix()));

	scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

	if (currentScene == nullptr) {
		return;
	}

	// Colliders and triggers
	auto colliderGroup = ::aderite::Engine::getSceneManager()->getCurrentScene()->getEntityRegistry()
		.group<scene::components::CollidersComponent>(
			entt::get<scene::components::TransformComponent>);
	for (auto entity : colliderGroup) {
		auto [colliders, transform] = colliderGroup.get<
			scene::components::CollidersComponent,
			scene::components::TransformComponent>(entity);

		for (physics::Collider* collider : *colliders.Colliders) {
			switch (static_cast<io::RuntimeSerializables>(collider->getType())) {
			case io::RuntimeSerializables::BOX_CLDR: {
				renderBoxCollider(static_cast<physics::BoxCollider*>(collider), transform);
				break;
			}
			}
		}
	}
}

void EditorRenderOperation::renderBoxCollider(physics::BoxCollider* collider, const scene::components::TransformComponent& transform) {
	glm::vec3 extents = collider->getSize();

	scene::components::TransformComponent tempTransform = transform;
	tempTransform.Scale *= extents;

	if (collider->isTrigger()) {
		wfColor[0] = 0.0f;
		wfColor[1] = 1.0f;
		wfColor[2] = 0.0f;
		wfOpacity = 1.0f;
	}
	else {
		wfColor[0] = 1.0f;
		wfColor[1] = 0.0f;
		wfColor[2] = 0.0f;
		wfOpacity = 1.0f;
	}
	updateUniform();

	uint64_t state = 0
		| BGFX_STATE_WRITE_RGB
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_CULL_CCW
		| BGFX_STATE_MSAA
		| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
		;

	bgfx::setTransform(glm::value_ptr(scene::components::TransformComponent::computeTransform(tempTransform)));
	bgfx::setState(state);

	bgfx::setVertexBuffer(0, m_bcCube);
	bgfx::setIndexBuffer(m_bcCubeIbo);
	bgfx::submit(c_ViewId, m_wireframeShader, 0);
}

void EditorRenderOperation::loadMeshes() {
	bgfx::VertexLayout cubeLayout;
	cubeLayout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color1, 3, bgfx::AttribType::Float)
		.end();

	m_bcCube = bgfx::createVertexBuffer(bgfx::makeRef(bcCubeVertices, sizeof(bcCubeVertices)), cubeLayout);
	m_bcCubeIbo = bgfx::createIndexBuffer(bgfx::makeRef(bcCubeIndices, sizeof(bcCubeIndices)));
}

void EditorRenderOperation::loadShaders() {
	//static const std::filesystem::path cwd = std::filesystem::current_path();
	//static const std::filesystem::path res = cwd / "res/";
	//static const std::filesystem::path shaders = res / "shaders/";
	//static const std::filesystem::path bin = res / "bin/";

	//// Compile
	//static const std::filesystem::path wfShader = shaders / "wireframe/";
	//compiler::Compiler::compileShaderSource(wfShader, bin);

	//// Load
	//std::vector<unsigned char> wffs = ::aderite::Engine::getAssetManager()->loadBinFile((bin / "wireframe.fs.bin").string());
	//std::vector<unsigned char> wfvs = ::aderite::Engine::getAssetManager()->loadBinFile((bin / "wireframe.vs.bin").string());

	//// Create memory objects
	//const bgfx::Memory* memWffs = bgfx::copy(wffs.data(), wffs.size() + 1);
	//memWffs->data[memWffs->size - 1] = '\0';
	//const bgfx::Memory* memWfvs = bgfx::copy(wfvs.data(), wfvs.size() + 1);
	//memWfvs->data[memWfvs->size - 1] = '\0';

	//// Create objects
	//bgfx::ShaderHandle vsh = bgfx::createShader(memWfvs);
	//bgfx::ShaderHandle fsh = bgfx::createShader(memWffs);

	//bgfx::setName(vsh, "Wireframe vertex shader");
	//bgfx::setName(fsh, "Wireframe fragment shader");

	//m_wireframeShader = bgfx::createProgram(vsh, fsh, true);
}

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
