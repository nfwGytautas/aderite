#pragma once

#include "aderite/Config.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/interfaces/IEngineMiddleware.hpp"

#include "aderite/asset/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/window/Forward.hpp"

#define ADERITE_SYSTEM_PTR(public_name, class_name, field_name) public: static class_name* public_name() { return ::aderite::Engine::get()->field_name; } private: class_name* field_name = nullptr;

ADERITE_ROOT_NAMESPACE_BEGIN

/**
 * @brief Main aderite engine instance
*/
class Engine {
public:
	/**
	 * @brief Engine init options
	*/
	struct InitOptions {
		
	};

public:
	/**
	 * @brief Returns the instance of the engine
	*/
	static Engine* get();

	/**
	 * @brief Initializes the aderite engine
	 * @param options Initialization options for the engine, some can be changed at runtime
	*/
	bool init(InitOptions options);

	/**
	 * @brief Shutdown aderite engine
	*/
	void shutdown();

	/**
	 * @brief Begins aderite engine loop
	*/
	void loop();

	/**
	 * @brief Request the engine to shutdown
	*/
	void requestExit();

	/**
	 * @brief Aborts a requested exit
	*/
	void abortExit();

	// TODO: Rethink this
	void beginFrame();
	void endFrame();

	/**
	 * @brief Function is invoked when the Renderer was initialized
	*/
	void onRendererInitialized();

	/**
	 * @brief Attaches a aderite editor instance to the runtime, previous one is deleted
	 * @param editor Editor to attach
	*/
	void attachMiddleware(interfaces::IEngineMiddleware* editor);
private:
	Engine() {}
	Engine(const Engine& o) = delete;

private:
	bool m_wantsToShutdown = false;
private:
	ADERITE_SYSTEM_PTR(getWindowManager, window::WindowManager, m_window_manager)
	ADERITE_SYSTEM_PTR(getRenderer, rendering::Renderer, m_renderer)
	ADERITE_SYSTEM_PTR(getAssetManager, asset::AssetManager, m_AssetManager)
	ADERITE_SYSTEM_PTR(getSceneManager, scene::SceneManager, m_scene_manager)

#if EDITOR_ENABLED == 1
	ADERITE_SYSTEM_PTR(getMiddleware, interfaces::IEngineMiddleware, m_editor)
#endif
};

ADERITE_ROOT_NAMESPACE_END
