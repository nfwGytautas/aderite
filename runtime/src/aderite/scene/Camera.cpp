#include "Camera.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace scene {

Camera::Camera() {
    const uint64_t flags = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP |
                           BGFX_SAMPLER_V_CLAMP | BGFX_TEXTURE_BLIT_DST;

    m_output = bgfx::createTexture2D(bgfx::BackbufferRatio::Equal, false, 1, bgfx::TextureFormat::BGRA8, flags);

    // Initial data
    this->updateMatrices();
}

Camera::~Camera() {
    bgfx::destroy(m_output);
}

const glm::vec3& Camera::getPosition() const {
    return m_position;
}

void Camera::setPosition(const glm::vec3& position) {
    m_position = position;
    this->updateMatrices();
}

const glm::vec3& Camera::getRotation() const {
    return m_eulerRotation;
}

void Camera::setRotation(const glm::vec3& rotation) {
    m_eulerRotation = rotation;
    this->updateMatrices();
}

const glm::mat4& Camera::getViewMatrix() const {
    return m_viewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix() const {
    return m_projectionMatrix;
}

bgfx::TextureHandle Camera::getOutputHandle() const {
    return m_output;
}

bool Camera::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Position" << YAML::Value << m_position;
    emitter << YAML::Key << "Rotation" << YAML::Value << m_eulerRotation;
    return true;
}

bool Camera::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_position = data["Position"].as<glm::vec3>();
    m_eulerRotation = data["Rotation"].as<glm::vec3>();
    return true;
}

reflection::Type Camera::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::CAMERA);
}

void Camera::updateMatrices() {
    // TODO: Aspect ratio

    bgfx::setName(m_output, this->getName().c_str());

    // Projection
    m_projectionMatrix = glm::perspective(glm::radians(m_fov), 1.0f, 0.1f, 1000.0f);

    // View
    glm::quat orientation = glm::quat(m_eulerRotation);
    m_viewMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
    m_viewMatrix = glm::inverse(m_viewMatrix);
}

} // namespace scene
} // namespace aderite
