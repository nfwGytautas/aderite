#include "Aderite.hpp"

#include <bx/timer.h>

#include "aderite/asset/AssetManager.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/reflection/Reflector.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"
#include "aderite/window/WindowManager.hpp"

#if MIDDLEWARE_ENABLED == 1
#define MIDDLEWARE_ACTION(action, ...) m_middleware->action(__VA_ARGS__);
#else
#define MIDDLEWARE_ACTION(action, ...)
#endif

namespace aderite {

Engine* Engine::get() {
    static Engine* instance = new Engine();
    return instance;
}

bool Engine::init(InitOptions options) {
    // First init logger
    Logger::get()->init();

    ADERITE_LOG_BLOCK;

    LOG_TRACE("Initializing aderite engine");
    LOG_DEBUG("Version: {0}", EngineVersion);

    // At this point there should be an editor if it is enabled
#if MIDDLEWARE_ENABLED == 1
    if (!m_middleware) {
        LOG_WARN("[Engine] Attaching null middleware, cause no middleware set");
        attachMiddleware(new interfaces::NullEditor());
    }
#endif

    // File handle
    m_fileHandler = new io::FileHandler();

    // Loader pool
    // TODO: Configurable thread count
    m_loaderPool = new io::LoaderPool(2);

    // Asset manager
    m_assetManager = new asset::AssetManager();
    if (!m_assetManager->init()) {
        LOG_ERROR("[Engine] Aborting aderite initialization");
        return false;
    }

    // Scene manager
    m_sceneManager = new scene::SceneManager();
    if (!m_sceneManager->init()) {
        LOG_ERROR("[Engine] Aborting aderite initialization");
        return false;
    }

    // Window system
    m_windowManager = new window::WindowManager();
    if (!m_windowManager->init()) {
        LOG_ERROR("[Engine] Aborting aderite initialization");
        return false;
    }

    // Input manager
    m_inputManager = new input::InputManager();
    if (!m_inputManager->init()) {
        LOG_ERROR("[Engine] Aborting aderite initialization");
        return false;
    }

    // Physics controller
    m_physicsController = new physics::PhysicsController();
    if (!m_physicsController->init()) {
        LOG_ERROR("[Engine] Aborting aderite initialization");
        return false;
    }

    // Audio controller
    m_audioController = new audio::AudioController();
    if (!m_audioController->init()) {
        LOG_ERROR("[Engine] Aborting aderite initialization");
        return false;
    }

    // Renderer
    m_renderer = new rendering::Renderer();
    if (!m_renderer->init()) {
        LOG_ERROR("[Engine] Aborting aderite initialization");
        return false;
    }

    // Serializer
    m_serializer = new io::Serializer();
    if (!m_serializer->init()) {
        LOG_ERROR("[Engine] Aborting aderite initialization");
        return false;
    }

    // Reflection library
    m_reflector = new reflection::Reflector();
    if (!m_reflector->init()) {
        LOG_ERROR("[Engine] Aborting aderite initialization");
        return false;
    }

    // Script manager
    m_scriptManager = new scripting::ScriptManager();
    if (!m_scriptManager->init()) {
        LOG_ERROR("[Engine] Aborting aderite initialization");
        return false;
    }

    LOG_INFO("[Engine] All Aderite engine systems initialized");
    LOG_TRACE("");

    // Transition to ready state
    this->setState(CurrentState::SYSTEM_UPDATE);

    MIDDLEWARE_ACTION(onRuntimeInitialized);

    return true;
}

void Engine::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Engine] Shutting down");
    MIDDLEWARE_ACTION(onRuntimeShutdown);

    m_scriptManager->shutdown();
    m_sceneManager->shutdown();
    m_audioController->shutdown();
    m_inputManager->shutdown();
    m_serializer->shutdown();
    m_reflector->shutdown();
    m_assetManager->shutdown();
    m_physicsController->shutdown();
    m_renderer->shutdown();
    m_windowManager->shutdown();

    delete m_sceneManager;
    delete m_physicsController;
    delete m_audioController;
    delete m_renderer;
    delete m_windowManager;
    delete m_inputManager;
    delete m_serializer;
    delete m_loaderPool;
    delete m_fileHandler;
    delete m_reflector;
    delete m_scriptManager;
    delete m_assetManager;

    delete m_middleware;

    LOG_INFO("[Engine] Engine shutdown");
}

void Engine::loop() {
    LOG_TRACE("[Engine] Entering engine loop");
    
    while (m_state != CurrentState::AWAITING_SHUTDOWN) {
        this->tick();
    }

    LOG_TRACE("[Engine] Exiting engine loop");
}

void Engine::tick() {
    static int64_t last = bx::getHPCounter();

    // Calculate delta
    int64_t now = bx::getHPCounter();
    const int64_t frameTime = now - last;
    last = now;
    const double freq = double(bx::getHPFrequency());
    const float delta = float(double(frameTime) / freq);

    // Updates
    switch (m_state) {
    case CurrentState::FULL: {
        m_physicsController->update(delta);
        MIDDLEWARE_ACTION(onPhysicsUpdate, delta);

        // Fall through
    }
    case CurrentState::LOGIC: {
        MIDDLEWARE_ACTION(onScriptUpdate, delta);

        // Fall through
    }
    case CurrentState::SYSTEM_UPDATE: {
        // Query events
        m_inputManager->update();

        // Update audio and flush queued audio commands to controller (FMOD should always update)
        m_audioController->update();

        // Asset manager
        m_assetManager->update();

        MIDDLEWARE_ACTION(onSystemUpdate, delta);

        // Fall through
    }
    case CurrentState::RENDER_ONLY: {
        // Scene
        scene::Scene* currentScene = m_sceneManager->getCurrentScene();
        if (currentScene != nullptr) {
            currentScene->update(delta);
        }
        break;
    }
    }

    // Rendering
    MIDDLEWARE_ACTION(onStartRender);
    m_renderer->render();
    MIDDLEWARE_ACTION(onPreRenderCommit);
    m_renderer->commit();
    MIDDLEWARE_ACTION(onEndRender);
}

void Engine::onRendererInitialized() const {
    MIDDLEWARE_ACTION(onRendererInitialized);
}

void Engine::onSceneChanged(scene::Scene* scene) const {
    MIDDLEWARE_ACTION(onSceneChanged, scene);
}

void Engine::onWindowResized(unsigned int newWidth, unsigned int newHeight) const {
    // TODO: Event system?
    m_renderer->onWindowResized(newWidth, newHeight);
}

void Engine::attachMiddleware(interfaces::IEngineMiddleware* middleware) {
    if (m_middleware != nullptr) {
        delete m_middleware;
    }

    m_middleware = middleware;
}

Engine::CurrentState Engine::getState() const {
    return m_state;
}

void Engine::setState(CurrentState state) {
    LOG_TRACE("[Engine] State transition from {0} to {1}", static_cast<int>(m_state), static_cast<int>(state));
    m_state = state;
}

} // namespace aderite
