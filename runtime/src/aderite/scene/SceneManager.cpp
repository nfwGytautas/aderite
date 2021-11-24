#include "SceneManager.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"

namespace aderite {
namespace scene {

bool SceneManager::init() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Scene] Initializing scene manager");
    LOG_INFO("[Scene] Scene manager initialized");
    return true;
}

void SceneManager::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Scene] Shutting down scene manager");
    LOG_INFO("[Scene] Scene manager shutdown");
}

void SceneManager::setActive(Scene* scene) {
    LOG_TRACE("[Scene] Setting active scene to {0}", scene->getHandle());

    // Notify engine
    ::aderite::Engine::get()->onSceneChanged(scene);
    m_activeScene = scene;

    LOG_INFO("[Scene] Active scene changed");
}

Scene* SceneManager::getCurrentScene() const {
    return m_activeScene;
}

} // namespace scene
} // namespace aderite
