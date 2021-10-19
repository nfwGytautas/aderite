#include "CameraProvideOperation.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"

namespace aderite {
namespace rendering {

void CameraProvideOperation::execute() {
	m_valid = false;

	// Resolve the camera to use

	// Get current scene
	scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
	if (!currentScene) {
		return;
	}

	// Entity group
	auto cameras = currentScene->getEntityRegistry().group<
		scene::components::CameraComponent
	>
		(
			entt::get<scene::components::TransformComponent>
			);

	for (auto entity : cameras) {
		auto [camera, transform] = cameras.get(entity);

		if (camera.Main) {
			// TODO: Calculate matrices
			m_viewMatrix = glm::mat4(1);
			m_projMatrix = glm::mat4(1);
			m_valid = true;
			return;
		}
	}
}

const glm::mat4& CameraProvideOperation::getViewMatrix() const {
	return m_viewMatrix;
}

const glm::mat4& CameraProvideOperation::getProjectionMatrix() const {
	return m_projMatrix;
}

bool CameraProvideOperation::isValid() const {
	return m_valid;
}

}
}
