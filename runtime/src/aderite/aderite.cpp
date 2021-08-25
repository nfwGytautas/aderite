#include "Aderite.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/window/WindowManager.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/scene/SceneManager.hpp"


#if EDITOR_ENABLED == 1
#define EDITOR_ACTION(action, ...) m_editor->action(__VA_ARGS__);
#else
#define EDITOR_ACTION(action, ...) 
#endif

namespace aderite {

	Engine* ::aderite::Engine::get() {
		static Engine* instance = new Engine();
		return instance;
	}

	bool ::aderite::Engine::init(InitOptions options) {
		// First init logger
		Logger::get()->init();

		LOG_TRACE("Initializing aderite engine");
		LOG_DEBUG("Version: {0}", EngineVersion);

		// Asset manager
		m_AssetManager = new asset::AssetManager();
		if (!m_AssetManager->init()) {
			LOG_ERROR("Aborting aderite initialization");
			return false;
		}

		// Scene manager
		m_scene_manager = new scene::SceneManager();
		if (!m_scene_manager->init()) {
			LOG_ERROR("Aborting aderite initialization");
			return false;
		}

		// Window system
		m_window_manager = new window::WindowManager();
		if (!m_window_manager->init()) {
			LOG_ERROR("Aborting aderite initialization");
			return false;
		}

		// Renderer
		m_renderer = rendering::Renderer::createInstance(); // Delay init for until there are windows

		// At this point there should be an editor if it is enabled
#if EDITOR_ENABLED == 1
		if (!m_editor) {
			attachMiddleware(new interfaces::NullEditor());
		}
#endif

		EDITOR_ACTION(onRuntimeInitialized);

		return true;
	}

	void ::aderite::Engine::shutdown() {
		EDITOR_ACTION(onRuntimeShutdown);

		m_scene_manager->shutdown();
		m_AssetManager->shutdown();
		m_renderer->shutdown();
		m_window_manager->shutdown();

		delete m_scene_manager;
		delete m_AssetManager;
		delete m_renderer;
		delete m_window_manager;
		delete m_editor;
	}

	void ::aderite::Engine::loop() {
		while (!m_wantsToShutdown) {
			EDITOR_ACTION(onBeginFrame);
			beginFrame();
			EDITOR_ACTION(onStartRender);
			m_renderer->render();
			EDITOR_ACTION(onEndRender);
			endFrame();
			EDITOR_ACTION(onEndFrame);
		}
	}

	void ::aderite::Engine::requestExit() {
		m_wantsToShutdown = true;
		EDITOR_ACTION(onRequestedExit);
	}

	void ::aderite::Engine::abortExit() {
		m_wantsToShutdown = false;
	}

	void ::aderite::Engine::beginFrame() {
		m_window_manager->beginFrame();
		m_renderer->beginFrame();
	}

	void ::aderite::Engine::endFrame() {
		m_renderer->endFrame();
		m_window_manager->endFrame();
	}

	void ::aderite::Engine::onRendererInitialized() {
		EDITOR_ACTION(onRendererInitialized);
	}

	void ::aderite::Engine::attachMiddleware(interfaces::IEngineMiddleware* editor) {
		if (m_editor != nullptr) {
			delete m_editor;
		}

		m_editor = editor;
	}

}