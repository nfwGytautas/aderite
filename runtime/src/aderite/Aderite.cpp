#include "Aderite.hpp"

#include <bx/timer.h>

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

    // Loader pool
    // TODO: Configurable thread count
    m_fileHandler = new io::FileHandler();
    m_loaderPool = new io::LoaderPool(2);

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

    delete m_middleware;

    LOG_INFO("[Engine] Engine shutdown");
}

void Engine::loop() {
    LOG_TRACE("[Engine] Entering engine loop");
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
        updateScenes(deltaTimeSec);
        updatePhysics(deltaTimeSec);
        updateScripts(deltaTimeSec);

        // Rendering
        MIDDLEWARE_ACTION(onStartRender);
        m_renderer->render();
        MIDDLEWARE_ACTION(onPreRenderCommit);
        m_renderer->commit();
        MIDDLEWARE_ACTION(onEndRender);
    }

    LOG_TRACE("[Engine] Exiting engine loop");
}

void Engine::requestExit() {
    LOG_TRACE("[Engine] Exit requested");
    m_wantsToShutdown = true;
    MIDDLEWARE_ACTION(onRequestedExit);
}

void Engine::abortExit() {
    LOG_TRACE("[Engine] Exit aborted");
    m_wantsToShutdown = false;
}

void Engine::onRendererInitialized() const {
    MIDDLEWARE_ACTION(onRendererInitialized);
}

void Engine::onPipelineChanged(rendering::Pipeline* pipeline) const {
    MIDDLEWARE_ACTION(onPipelineChanged, pipeline);
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

void Engine::startPhysicsUpdates() {
    LOG_TRACE("[Engine] Enabling physics updates");
    // Before starting physics updates reset the engine
    m_willUpdatePhysics = true;
}

void Engine::stopPhysicsUpdates() {
    LOG_TRACE("[Engine] Disabling physics updates");
    m_willUpdatePhysics = false;
}

void Engine::startScriptUpdates() {
    LOG_TRACE("[Engine] Enabling script updates");
    m_willUpdateScripts = true;
}

void Engine::stopScriptUpdates() {
    LOG_TRACE("[Engine] Disabling script updates");
    m_willUpdateScripts = false;
}

void Engine::startSceneUpdates() {
    LOG_TRACE("[Engine] Enabling scene updates");
    m_willUpdateScenes = true;
}

void Engine::stopSceneUpdates() {
    LOG_TRACE("[Engine] Disabling scene updates");
    m_willUpdateScenes = false;
}

void Engine::updateSystem(float delta) const {
    // Query events
    m_inputManager->update();

    // Update audio and flush queued audio commands to controller (FMOD should always update)
    m_audioController->update();

    MIDDLEWARE_ACTION(onSystemUpdate, delta);
}

void Engine::updateScenes(float delta) const {
    if (!m_willUpdateScenes) {
        return;
    }

    scene::Scene* currentScene = m_sceneManager->getCurrentScene();
    currentScene->update(delta);

    MIDDLEWARE_ACTION(onSceneUpdate, delta);
}

void Engine::updatePhysics(float delta) const {
    if (!m_willUpdatePhysics) {
        return;
    }

    m_physicsController->update(delta);

    MIDDLEWARE_ACTION(onPhysicsUpdate, delta);
}

void Engine::updateScripts(float delta) const {
    if (!m_willUpdateScripts) {
        return;
    }

    m_scriptManager->update(delta);

    MIDDLEWARE_ACTION(onScriptUpdate, delta);
}

} // namespace aderite
