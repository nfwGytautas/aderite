#include "SceneManager.hpp"

#include <thread>
#include "aderite/Aderite.hpp"
#include "aderite/utility/Random.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/scene/Scene.hpp"

ADERITE_SCENE_NAMESPACE_BEGIN
		
bool SceneManager::init() {
	return true;
}

void SceneManager::shutdown() {

}

void SceneManager::setActive(Scene* scene) {
	// Notify engine
	::aderite::Engine::get()->onSceneChanged(scene);
	m_activeScene = scene;
}

Scene* SceneManager::getCurrentScene() const {
	return m_activeScene;
}

ADERITE_SCENE_NAMESPACE_END
