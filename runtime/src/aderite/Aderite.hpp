#pragma once

#include "aderite/Config.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/interfaces/IEngineMiddleware.hpp"

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/input/Forward.hpp"
#include "aderite/physics/Forward.hpp"
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

	/**
	 * @brief Function is invoked when the Renderer was initialized
	*/
	void onRendererInitialized();

	/**
	 * @brief Function is invoked when the active scene was changed
	*/
	void onSceneChanged(scene::Scene* scene);

	/**
	 * @brief Function invoked by input manager when the windows has been resized
	 * @param newWidth New width of the window
	 * @param newHeight New height of the window
	*/
	void onWindowResized(unsigned int newWidth, unsigned int newHeight);

	/**
	 * @brief Attaches a aderite middleware instance to the runtime, previous one is deleted
	 * @param middleware Middleware to attach
	*/
	void attachMiddleware(interfaces::IEngineMiddleware* middleware);

	/**
	 * @brief Starts physics updates
	*/
	void startPhysicsUpdates();

	/**
	 * @brief Stops physics updates
	*/
	void stopPhysicsUpdates();

	/**
	 * @brief Starts scripts updates
	*/
	void startScriptUpdates();

	/**
	 * @brief Stops scripts updates
	*/
	void stopScriptUpdates();

	/**
	 * @brief Starts scene updates
	*/
	void startSceneUpdates();

	/**
	 * @brief Stops scene updates
	*/
	void stopSceneUpdates();
private:
	Engine() {}
	Engine(const Engine& o) = delete;

	void updateSystem(float delta);
	void updateScenes(float delta);
	void updatePhysics(float delta);
	void updateScripts(float delta);
private:
	bool m_wantsToShutdown = false;
	bool m_willUpdateScenes = true;
	bool m_willUpdatePhysics = true;
	bool m_willUpdateScripts = true;
private:
	ADERITE_SYSTEM_PTR(getWindowManager, window::WindowManager, m_windowManager)
	ADERITE_SYSTEM_PTR(getRenderer, rendering::Renderer, m_renderer)
	ADERITE_SYSTEM_PTR(getAssetManager, asset::AssetManager, m_assetManager)
	ADERITE_SYSTEM_PTR(getSceneManager, scene::SceneManager, m_sceneManager)
	ADERITE_SYSTEM_PTR(getInputManager, input::InputManager, m_inputManager)
	ADERITE_SYSTEM_PTR(getPhysicsController, physics::PhysicsController, m_physicsController)
	ADERITE_SYSTEM_PTR(getAudioController, audio::AudioController, m_audioController)

#if MIDDLEWARE_ENABLED == 1
	ADERITE_SYSTEM_PTR(getMiddleware, interfaces::IEngineMiddleware, m_middleware)
#endif
};

ADERITE_ROOT_NAMESPACE_END
