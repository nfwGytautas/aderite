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
}

Camera::~Camera() {
    bgfx::destroy(m_output);
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
    return true;
}

bool Camera::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    return true;
}

} // namespace scene
} // namespace aderite
