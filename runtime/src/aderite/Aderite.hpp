#pragma once

#include "aderite/Config.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/input/Forward.hpp"
#include "aderite/interfaces/IEngineMiddleware.hpp"
#include "aderite/io/Forward.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/reflection/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/window/Forward.hpp"

#define ADERITE_SYSTEM_PTR(public_name, class_name, field_name) \
public:                                                         \
    static class_name* public_name() {                          \
        return ::aderite::Engine::get()->field_name;            \
    }                                                           \
                                                                \
private:                                                        \
    class_name* field_name = nullptr;

namespace aderite {

/**
 * @brief Main aderite engine instance
 */
class Engine final {
public:
    /**
     * @brief Engine init options
     */
    struct InitOptions {};

    /**
     * @brief Enum representing the current engine state
     */
    enum class CurrentState {
        INIT,              // The initial state of the engine
        AWAITING_SHUTDOWN, // Shutdown was requested and the engine is now waiting for systems to shutdown before closing
        RENDER_ONLY,       // Only renders the current information, no updates are issued
        SYSTEM_UPDATE,     // System updates such as IO is done, while game loop and the physics simulation is paused
        LOGIC,             // Performs updates and renders, but physics are not simulated
        FULL,              // Updates including simulation of physics is done and the changes are rendered
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
     * @brief Function is invoked when the Renderer was initialized
     */
    void onRendererInitialized() const;

    /**
     * @brief Function is invoked when the active scene was changed
     */
    void onSceneChanged(scene::Scene* scene) const;

    /**
     * @brief Function invoked by input manager when the windows has been resized
     * @param newWidth New width of the window
     * @param newHeight New height of the window
     */
    void onWindowResized(unsigned int newWidth, unsigned int newHeight) const;

    /**
     * @brief Attaches a aderite middleware instance to the runtime, previous one is deleted
     * @param middleware Middleware to attach
     */
    void attachMiddleware(interfaces::IEngineMiddleware* middleware);

    /**
     * @brief Returns the current state of the engine
     */
    CurrentState getState() const;

    /**
     * @brief Transitions the engine to the specified state
     * @param state New state of the engine
     */
    void setState(CurrentState state);

private:
    Engine() {}
    Engine(const Engine& o) = delete;

private:
    CurrentState m_state = CurrentState::INIT;

private:
    ADERITE_SYSTEM_PTR(getWindowManager, window::WindowManager, m_windowManager)
    ADERITE_SYSTEM_PTR(getRenderer, rendering::Renderer, m_renderer)
    ADERITE_SYSTEM_PTR(getSceneManager, scene::SceneManager, m_sceneManager)
    ADERITE_SYSTEM_PTR(getInputManager, input::InputManager, m_inputManager)
    ADERITE_SYSTEM_PTR(getFileHandler, io::FileHandler, m_fileHandler)
    ADERITE_SYSTEM_PTR(getSerializer, io::Serializer, m_serializer)
    ADERITE_SYSTEM_PTR(getReflector, reflection::Reflector, m_reflector)
    ADERITE_SYSTEM_PTR(getLoaderPool, io::LoaderPool, m_loaderPool)
    ADERITE_SYSTEM_PTR(getPhysicsController, physics::PhysicsController, m_physicsController)
    ADERITE_SYSTEM_PTR(getAudioController, audio::AudioController, m_audioController)
    ADERITE_SYSTEM_PTR(getScriptManager, scripting::ScriptManager, m_scriptManager)
    ADERITE_SYSTEM_PTR(getAssetManager, asset::AssetManager, m_assetManager)

#if MIDDLEWARE_ENABLED == 1
    ADERITE_SYSTEM_PTR(getMiddleware, interfaces::IEngineMiddleware, m_middleware)
#endif
};

} // namespace aderite
