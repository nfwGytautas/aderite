#include "CameraProvideOperation.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/EntityCamera.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

interfaces::ICamera* CameraProvideOperation::getCamera() const {
	return p_camera;
}

void MainCameraProvideOperation::execute() {
	// Resolve the camera to use
	
	// Get current scene
	scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
	if (!currentScene) {
		p_camera = nullptr;
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
			p_camera = static_cast<interfaces::ICamera*>(camera.Camera);
			return;
		}
	}
}

void FreeCameraProvideOperation::setCamera(interfaces::ICamera* camera) {
	p_camera = camera;
}

ADERITE_RENDERING_NAMESPACE_END
