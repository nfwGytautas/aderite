#include "SceneManager.hpp"

#include <thread>
#include "aderite/aderite.hpp"
#include "aderite/utility/random.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/scene/scene.hpp"

ADERITE_SCENE_NAMESPACE_BEGIN
		
bool SceneManager::init() {
	return true;
}

void SceneManager::shutdown() {

}

void SceneManager::set_active(Scene* scene) {
	if (scene->is_loaded()) {
		m_activeScene = scene;
		return;
	}

	// Check if it's preparing to be loaded if not then prepare
	if (!scene->is_preparing()) {
		scene->prepare_load();
	}

	// Wait until the scene 
	while (!scene->ready_to_load()) {
		// TODO: Rethink this
		// Sleep for 1 second
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	if (!scene->is_loaded()) {
		scene->load();
	}

	m_activeScene = scene;
}

Scene* SceneManager::current_scene() const {
	return m_activeScene;
}

ADERITE_SCENE_NAMESPACE_END
