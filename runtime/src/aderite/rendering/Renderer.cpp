#include "Renderer.hpp"

#include <unordered_map>

#include <glm/gtc/type_ptr.hpp>

#include <bgfx/bgfx.h>
#include <bx/string.h>

#include "aderite/Config.hpp"
#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/bgfx.hpp"
#include "aderite/utility/Utility.hpp"
#include "aderite/window/WindowManager.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/ShaderAsset.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/EntityCamera.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/rendering/DrawCall.hpp"

// Passes
#include "aderite/rendering/pass/SkyPass.hpp"
#include "aderite/rendering/pass/IblPass.hpp"

#if DEBUG_RENDER == 1
#include "aderite/rendering/pass/DebugPass.hpp"
#endif

// Uniforms
#include "aderite/rendering/uniform/UniformManager.hpp"

namespace impl {

	/**
	 * @brief bgfx logging callback
	*/
	class bgfxCallback : public bgfx::CallbackI
	{
	public:
		virtual ~bgfxCallback() { }
		virtual void fatal(const char* filePath, uint16_t line, bgfx::Fatal::Enum code, const char* str) override {
			if (code != bgfx::Fatal::DebugCheck) {
				LOG_FATAL("{0}", str);
			}
		}

		virtual void traceVargs(const char* filePath, uint16_t line, const char* format, va_list args) override {
			char buffer[1024];
			int32_t written = bx::vsnprintf(buffer, BX_COUNTOF(buffer), format, args);
			if (written > 0 && written < BX_COUNTOF(buffer))
			{
				// bgfx sends lines with newlines, spdlog adds another
				if (buffer[written - 1] == '\n')
					buffer[written - 1] = '\0';
				LOG_TRACE(buffer);
			}
		}

		virtual void profilerBegin(const char*, uint32_t, const char*, uint16_t) override { }
		virtual void profilerBeginLiteral(const char*, uint32_t, const char*, uint16_t) override { }
		virtual void profilerEnd() override { }
		virtual uint32_t cacheReadSize(uint64_t) override {
			return 0;
		}
		virtual bool cacheRead(uint64_t, void*, uint32_t) override {
			return false;
		}
		virtual void cacheWrite(uint64_t, const void*, uint32_t) override { }
		virtual void captureBegin(uint32_t, uint32_t, uint32_t, bgfx::TextureFormat::Enum, bool) override { }
		virtual void captureEnd() override { }
		virtual void captureFrame(const void*, uint32_t) override { }
		virtual void screenShot(const char*, uint32_t, uint32_t, uint32_t, const void*, uint32_t, bool yflip) override { }
	};

	static bgfxCallback g_cb;
}

ADERITE_RENDERING_NAMESPACE_BEGIN

bool Renderer::init() {
	LOG_DEBUG("BGFX Renderer");

	auto windowManager = ::aderite::Engine::getWindowManager();
	glm::i32vec2 size = windowManager->getSize();

	// Platform data
	bgfx::PlatformData pd;
	pd.nwh = windowManager->getNativeHandle();

	bgfx::Init bgfxInit;
	bgfxInit.platformData = pd;
	bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a backend
	bgfxInit.resolution.width = size.x;
	bgfxInit.resolution.height = size.y;
	bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
	bgfxInit.callback = &::impl::g_cb;

	// TODO: Add multi threaded

	if (!bgfx::init(bgfxInit)) {
		LOG_ERROR("Failed to initialize BGFX");
	}

	// Now create some defaults


	// Create uniforms
	m_uManager = new uniform::UniformManager();

	// TODO: Error check
	m_uManager->init();

	// Create passes
	m_passes[0] = new pass::SkyPass();
	m_passes[1] = new pass::IblPass();

#if DEBUG_RENDER == 1
	m_debugPass = new pass::DebugPass();
	m_debugPass->init(m_uManager);
#endif

	for (uint8_t i = 0; i < c_NumPasses; i++) {
		m_passes[i]->init(m_uManager);
	}
		
	// Clear color
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x252525FF, 1.0f, 0);

	// Initial view rect
	auto windowSize = ::aderite::Engine::getWindowManager()->getSize();
	onWindowResized(windowSize.x, windowSize.y, false);

	// Finish any queued operations
	bgfx::frame();

	m_isInitialized = true;
	::aderite::Engine::get()->onRendererInitialized();
	return true;
}

void Renderer::shutdown() {
#if DEBUG_RENDER == 1
	m_debugPass->shutdown();
	delete m_debugPass;
#endif

	for (uint8_t i = 0; i < c_NumPasses; i++) {
		m_passes[i]->shutdown();
		delete m_passes[i];
	}

	m_uManager->shutdown();
	delete m_uManager;

	bgfx::shutdown();
}

void Renderer::setVsync(bool enabled) {
	ADERITE_UNIMPLEMENTED;
}

void Renderer::onWindowResized(unsigned int newWidth, unsigned int newHeight, bool reset) {
	setResolution(glm::uvec2(newWidth, newHeight));
#if DEBUG_RENDER == 1
	// TODO: Event system?
	m_debugPass->onWindowResized(newWidth, newHeight);
#endif

	for (uint8_t i = 0; i < c_NumPasses; i++) {
		m_passes[i]->onWindowResized(newWidth, newHeight);
	}

	if (reset) {
		bgfx::reset(newWidth, newHeight);
	}
}

void Renderer::renderScene(scene::Scene* scene) {
	if (scene == nullptr) {
		return;
	}

	if (!isReady()) {
		return;
	}

	// Skip rendering if there are no draw calls
#if MIDDLEWARE_ENABLED == 1
	interfaces::ICamera* middlewareCamera = scene->getMiddlewareCamera();
	if (middlewareCamera && middlewareCamera->isEnabled()) {
		renderFrom(scene, middlewareCamera);

#if DEBUG_RENDER == 1
		m_debugPass->pass(middlewareCamera);
		bgfx::discard(BGFX_DISCARD_ALL);
#endif
	}
#endif

	// Execute draw calls for all cameras
	auto cameraGroup = scene->getEntityRegistry()
		.group<scene::components::CameraComponent>(
			entt::get<scene::components::TransformComponent>);
	for (auto entity : cameraGroup) {
		auto [camera, transform] = cameraGroup.get(entity);
		if (!camera.Camera->isEnabled()) {
			// Skip disabled cameras
			continue;
		}

		renderFrom(scene, camera.Camera);
	}

	bgfx::discard(BGFX_DISCARD_ALL);
}

bool Renderer::isReady() {
	return m_isInitialized;
}

void Renderer::setResolution(const glm::uvec2& size) {
	bgfx::setViewRect(0, 0, 0, size.x, size.y);

	// TODO: Forward to render passes	
}

void Renderer::displayFrame(bgfx::FrameBufferHandle image) {
	LOG_ERROR("displayFrame() not implemented yet");
}

void Renderer::commit() {
	// Commit
	bgfx::frame(false);
	//const bgfx::Stats* stats = bgfx::getStats();
	//LOG_INFO("Commiting {0} draw calls", stats->numDraw);
}

uniform::UniformManager* Renderer::getUniformManager() const {
	return m_uManager;
}

void Renderer::renderFrom(scene::Scene* scene, interfaces::ICamera* eye) {
	DrawCallList drawcalls;
	std::unordered_map<size_t, DrawCallList::iterator> lookup;

	// Entity group
	auto renderables = scene->getEntityRegistry().group<
		scene::components::MeshRendererComponent, 
		scene::components::TransformComponent
	>
	(
		entt::get<scene::components::MetaComponent>
	);

	for (auto e : renderables) {
		auto [meshRenderer, transform, meta] = renderables.get(e);
		
		// Validate
		if (!validateEntity(meta, meshRenderer)) {
			continue;
		}

		// Cull
		if (!frustumCull(eye, transform)) {
			continue;
		}

		// Check if draw call already exists
		size_t hash = utility::combineHash(meshRenderer.MeshHandle->hash(), meshRenderer.MaterialHandle->hash());

		if (lookup.find(hash) == lookup.end()) {
			// Create new drawcall and insert
			lookup[hash] = drawcalls.insert(drawcalls.end(), DrawCall{
				meshRenderer.MeshHandle->getVboHandle(),
				meshRenderer.MeshHandle->getIboHandle(),
				meshRenderer.MaterialHandle->getFields().Type->getShaderHandle(),
				meshRenderer.MaterialHandle->getFields().Type->getUniformHandle(),
				meshRenderer.MaterialHandle->getSamplerData(),
				meshRenderer.MaterialHandle->getPropertyData(),
				std::vector<scene::components::TransformComponent*>()
			});
		}

		(*lookup[hash]).Transformations.push_back(&transform);
	}

	// Occlusion cull
	occlusionCull(drawcalls);

	// Render
	bgfx::setViewFrameBuffer(0, eye->getOutputHandle());
	bgfx::touch(0);

	// Set persistent matrices
	bgfx::setViewTransform(
		0,
		glm::value_ptr(eye->computeViewMatrix()),
		glm::value_ptr(eye->computeProjectionMatrix()));

	for (DrawCall& dc : drawcalls) {
		optimize(dc);
		render(dc);
	}

	// Discard this draw call information
	bgfx::discard(BGFX_DISCARD_ALL);
}

bool Renderer::validateEntity(scene::components::MetaComponent& meta, scene::components::MeshRendererComponent& mrenderer) {
	// TODO: Check if enabled

	asset::MeshAsset* mesh = mrenderer.MeshHandle;
	asset::MaterialAsset* material = mrenderer.MaterialHandle;

	if (mesh == nullptr ||
		material == nullptr) {
		return false;
	}

	asset::MaterialTypeAsset* materialType = material->getFields().Type;

	if (materialType == nullptr) {
		return false;
	}

	if (material->getPropertyData() == nullptr) {
		LOG_WARN("nullptr property data for {0}", meta.Name);
		return false;
	}

	// Check if assets are loaded
	if (!mesh->isLoaded() ||
		!material->isLoaded()) {
		// This shouldn't really happen, one of the systems are lagging behind
		LOG_WARN("Unloaded asset rendering for {0}", meta.Name);
		return false;
	}
	
	// Check if they are valid
	if (!bgfx::isValid(mesh->getVboHandle()) ||
		!bgfx::isValid(mesh->getIboHandle())) {
		LOG_WARN("Invalid mesh passed for {0}", meta.Name);
		return false;
	}

	if (!bgfx::isValid(materialType->getShaderHandle()) ||
		!bgfx::isValid(materialType->getUniformHandle())) {
		LOG_WARN("Invalid material passed for {0}", meta.Name);
		return false;
	}

	return true;
}

bool Renderer::frustumCull(interfaces::ICamera* eye, scene::components::TransformComponent& transform) {
	// TODO: Frustum cull

	return true;
}

void Renderer::occlusionCull(DrawCallList& dcl) {
	// TODO: Implement
}

void Renderer::optimize(DrawCall& dc) {
	if (dc.FullyCulled) {
		dc.Skip = true;
		return;
	}

	// Pack the transformations
	size_t moveCount = 0;
	for (size_t i = 0; i < dc.Transformations.size(); i++) {
		if (dc.Transformations[i] == nullptr) {
			moveCount += 1;
		}
		else {
			dc.Transformations[i - moveCount] = dc.Transformations[i];
		}
	}

	// Erase left over
	dc.Transformations.erase(dc.Transformations.end() - moveCount, dc.Transformations.end());
}

void Renderer::render(const DrawCall& dc) {
	// Check if valid draw call
	if (dc.Skip) {
		return;
	}

	// TODO: Output target from material type

	// Uniform
	bgfx::setUniform(dc.MaterialUniform, dc.UniformData, UINT16_MAX);

	// Samplers
	for (size_t i = 0; i < dc.Samplers.size(); i++) {
		bgfx::setTexture(i, dc.Samplers[i].first, dc.Samplers[i].second);
	}

	// Bind buffers
	bgfx::setVertexBuffer(0, dc.VBO);
	bgfx::setIndexBuffer(dc.IBO);

	uint64_t state = 0
		| BGFX_STATE_WRITE_R
		| BGFX_STATE_WRITE_G
		| BGFX_STATE_WRITE_B
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_CULL_CCW
		| BGFX_STATE_MSAA
		;

	bgfx::setState(state);

	for (auto& transform : dc.Transformations) {
		bgfx::setTransform(glm::value_ptr(scene::components::TransformComponent::computeTransform(*transform)));

		// Submit draw call
		uint8_t flags = BGFX_DISCARD_ALL & ~(BGFX_DISCARD_BINDINGS | BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS | BGFX_DISCARD_STATE);
		bgfx::submit(0, dc.Shader, 0, flags);
	}
}

ADERITE_RENDERING_NAMESPACE_END
