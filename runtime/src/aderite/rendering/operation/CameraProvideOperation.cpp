#include "CameraProvideOperation.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/rendering/PipelineState.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/components/Transform.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace rendering {

void CameraProvideOperation::execute(PipelineState* state) {
    // Resolve the camera to use

    // Get current scene
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
    if (!currentScene) {
        return;
    }

    // Entity group
    auto cameras = currentScene->getEntityRegistry().group<scene::CameraComponent>(entt::get<scene::TransformComponent>);

    for (auto entity : cameras) {
        auto [camera, transform] = cameras.get(entity);

        if (camera.Main) {
            // TODO: Calculate matrices
            m_viewMatrix = glm::mat4(1);
            m_projMatrix = glm::mat4(1);

            state->pushEye({m_viewMatrix, m_projMatrix});
            return;
        }
    }

    // Push invalid eye
    state->pushEye({glm::mat4(1), glm::mat4(1), false});
}

reflection::Type CameraProvideOperation::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::OP_CAMERA);
}

bool CameraProvideOperation::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    return true;
}

bool CameraProvideOperation::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    return true;
}

} // namespace rendering
} // namespace aderite
