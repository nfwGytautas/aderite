#include "StandaloneTransformProvider.hpp"

namespace aderite {
namespace scene {

const glm::vec3& StandaloneTransformProvider::getPosition() const {
    return m_position;
}

const glm::quat& StandaloneTransformProvider::getRotation() const {
    return m_rotation;
}

const glm::vec3& StandaloneTransformProvider::getScale() const {
    return m_scale;
}

void StandaloneTransformProvider::setPosition(const glm::vec3& position) {
    m_position = position;
}

void StandaloneTransformProvider::setRotation(const glm::quat& rotation) {
    m_rotation = rotation;
}

void StandaloneTransformProvider::setScale(const glm::vec3& scale) {
    m_scale = scale;
}

}
}
