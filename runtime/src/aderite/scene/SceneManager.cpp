#include "SceneManager.hpp"
#include <thread>

#include "aderite/Aderite.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Random.hpp"

namespace aderite {
namespace scene {

bool SceneManager::init() {
    return true;
}

void SceneManager::shutdown() {}

void SceneManager::setActive(Scene* scene) {
    // Notify engine
    ::aderite::Engine::get()->onSceneChanged(scene);
    m_activeScene = scene;
}

Scene* SceneManager::getCurrentScene() const {
    return m_activeScene;
}

} // namespace scene
} // namespace aderite
