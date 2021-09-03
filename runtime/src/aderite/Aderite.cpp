#include "Aderite.hpp"

#include <bx/timer.h>

#include "aderite/utility/Log.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/window/WindowManager.hpp"


#if MIDDLEWARE_ENABLED == 1
#define MIDDLEWARE_ACTION(action, ...) m_middleware->action(__VA_ARGS__);
#else
#define MIDDLEWARE_ACTION(action, ...) 
#endif

ADERITE_ROOT_NAMESPACE_BEGIN

Engine* Engine::get() {
	static Engine* instance = new Engine();
	return instance;
}

bool Engine::init(InitOptions options) {
	// First init logger
	Logger::get()->init();

	LOG_TRACE("Initializing aderite engine");
	LOG_DEBUG("Version: {0}", EngineVersion);

	// Asset manager
	m_assetManager = new asset::AssetManager();
	if (!m_assetManager->init()) {
		LOG_ERROR("Aborting aderite initialization");
		return false;
	}

	// Scene manager
	m_sceneManager = new scene::SceneManager();
	if (!m_sceneManager->init()) {
		LOG_ERROR("Aborting aderite initialization");
		return false;
	}

	// Window system
	m_windowManager = new window::WindowManager();
	if (!m_windowManager->init()) {
		LOG_ERROR("Aborting aderite initialization");
		return false;
	}

	// Input manager
	m_inputManager = new input::InputManager();
	if (!m_inputManager->init()) {
		LOG_ERROR("Aborting aderite initialization");
		return false;
	}

	// Physics controller
	m_physicsController = new physics::PhysicsController();
	if (!m_physicsController->init()) {
		LOG_ERROR("Aborting aderite initialization");
		return false;
	}

	// Audio controller
	m_audioController = new audio::AudioController();
	if (!m_audioController->init()) {
		LOG_ERROR("Aborting aderite initialization");
		return false;
	}

	// Renderer
	m_renderer = new rendering::Renderer();
	if (!m_renderer->init()) {
		LOG_ERROR("Aborting aderite initialization");
		return false;
	}

	// At this point there should be an editor if it is enabled
#if MIDDLEWARE_ENABLED == 1
	if (!m_middleware) {
		attachMiddleware(new interfaces::NullEditor());
	}
#endif

	MIDDLEWARE_ACTION(onRuntimeInitialized);

	return true;
}

void Engine::shutdown() {
	MIDDLEWARE_ACTION(onRuntimeShutdown);

	m_sceneManager->shutdown();
	m_assetManager->shutdown();
	m_physicsController->shutdown();
	m_audioController->shutdown();
	m_renderer->shutdown();
	m_windowManager->shutdown();
	m_inputManager->shutdown();

	delete m_sceneManager;
	delete m_physicsController;
	delete m_audioController;
	delete m_assetManager;
	delete m_renderer;
	delete m_windowManager;
	delete m_inputManager;
	delete m_middleware;
}

void Engine::loop() {
	static int64_t last = bx::getHPCounter();

	while (!m_wantsToShutdown) {
		// Calculate delta
		int64_t now = bx::getHPCounter();
		const int64_t frameTime = now - last;
		last = now;
		const double freq = double(bx::getHPFrequency());
		const float deltaTimeSec = float(double(frameTime) / freq);

		// Updates
		updateSystem(deltaTimeSec);
		updatePhysics(deltaTimeSec);
		updateScripts(deltaTimeSec);

		// Rendering
		scene::Scene* currentScene = m_sceneManager->getCurrentScene();
		MIDDLEWARE_ACTION(onStartRender);
		m_renderer->renderScene(currentScene);
		MIDDLEWARE_ACTION(onPreRenderCommit);
		m_renderer->commit();
		MIDDLEWARE_ACTION(onEndRender);
	}
}

void Engine::requestExit() {
	m_wantsToShutdown = true;
	MIDDLEWARE_ACTION(onRequestedExit);
}

void Engine::abortExit() {
	m_wantsToShutdown = false;
}

void Engine::onRendererInitialized() {
	MIDDLEWARE_ACTION(onRendererInitialized);
}

void Engine::onSceneChanged(scene::Scene* scene) {
	MIDDLEWARE_ACTION(onSceneChanged, scene);
}

void Engine::onWindowResized(unsigned int newWidth, unsigned int newHeight) {
	// TODO: Event system?
	m_renderer->onWindowResized(newWidth, newHeight);
}

void Engine::attachMiddleware(interfaces::IEngineMiddleware* middleware) {
	if (m_middleware != nullptr) {
		delete m_middleware;
	}

	m_middleware = middleware;
}

void Engine::startPhysicsUpdates() {
	// Before starting physics updates reset the engine
	m_willUpdatePhysics = true;
}

void Engine::stopPhysicsUpdates() {
	m_willUpdatePhysics = false;
}

void Engine::startScriptUpdates() {
	m_willUpdateScripts = true;
}

void Engine::stopScriptUpdates() {
	m_willUpdateScripts = false;
}

void Engine::startSceneUpdates() {
	m_willUpdateScenes = true;
}

void Engine::stopSceneUpdates() {
	m_willUpdateScenes = false;
}

void Engine::updateSystem(float delta) {
	// Query events
	m_inputManager->update();

	// Update audio and flush queued audio commands to controller (FMOD should always update)
	m_audioController->update();

	MIDDLEWARE_ACTION(onSystemUpdate, delta);
}

void Engine::updateScenes(float delta) {
	if (!m_willUpdateScenes) {
		return;
	}

	scene::Scene* currentScene = m_sceneManager->getCurrentScene();
	currentScene->update(delta);

	MIDDLEWARE_ACTION(onSceneUpdate, delta);
}

void Engine::updatePhysics(float delta) {
	if (!m_willUpdatePhysics) {
		return;
	}

	m_physicsController->update(delta);

	MIDDLEWARE_ACTION(onPhysicsUpdate, delta);
}

void Engine::updateScripts(float delta) {
	if (!m_willUpdateScripts) {
		return;
	}

	MIDDLEWARE_ACTION(onScriptUpdate, delta);
}

ADERITE_ROOT_NAMESPACE_END