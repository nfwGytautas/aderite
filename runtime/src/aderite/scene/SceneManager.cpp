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
	if (scene->isLoaded()) {
		m_activeScene = scene;
		return;
	}

	// Check if it's preparing to be loaded if not then prepare
	if (!scene->isPreparing()) {
		scene->prepareLoad();
	}

	// Wait until the scene 
	while (!scene->isReadyToLoad()) {
		// TODO: Rethink this
		// Sleep for 1 second
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	if (!scene->isLoaded()) {
		scene->load();
	}

	m_activeScene = scene;
}

Scene* SceneManager::getCurrentScene() const {
	return m_activeScene;
}

ADERITE_SCENE_NAMESPACE_END
