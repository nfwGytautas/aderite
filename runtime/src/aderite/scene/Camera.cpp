#include "Camera.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/Aderite.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/TransformProvider.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace scene {

Camera::Camera(scene::GameObject* gObject) : m_gObject(gObject) {
    const uint64_t flags = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP |
                           BGFX_SAMPLER_V_CLAMP | BGFX_TEXTURE_BLIT_DST;

    m_output = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, false, 1, bgfx::TextureFormat::BGRA8, flags);
}

Camera::~Camera() {
    bgfx::destroy(m_output);
}

void Camera::update(float delta) {
    scene::TransformProvider* const transform = m_gObject->getTransform();

    if (transform == nullptr) {
        return;
    }

    rendering::FrameData& fd = ::aderite::Engine::getRenderer()->getWriteFrameData();

    // Fill camera data
    rendering::CameraData cd;
    cd.Name = m_gObject->getName();
    cd.Output = m_output;
    cd.ProjectionMatrix = glm::perspective(glm::radians(m_fov), 1.0f, 0.1f, 1000.0f);
    cd.ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), transform->getPosition()) * glm::toMat4(transform->getRotation()));

    // Push to the list
    fd.Cameras.push_back(cd);
}

float Camera::getFoV() const {
    return m_fov;
}

void Camera::setFoV(float fov) {
    m_fov = fov;
}

bgfx::TextureHandle Camera::getOutputHandle() const {
    return m_output;
}

bool Camera::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Camera" << YAML::BeginMap;
    emitter << YAML::Key << "FoV" << YAML::Value << m_fov;
    emitter << YAML::EndMap;
    return true;
}

bool Camera::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& camera = data["Camera"];
    if (!camera || camera.IsNull()) {
        return false;
    }

    m_fov = camera["FoV"].as<float>();

    return true;
}

} // namespace scene
} // namespace aderite
