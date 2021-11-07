#include "Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace scene {

glm::mat4 Transform::computeTransform() const {
    glm::mat4 rotation = glm::toMat4(m_rotation);
    return glm::translate(glm::mat4(1.0f), m_position) * rotation * glm::scale(glm::mat4(1.0f), m_scale);
}

reflection::Type Transform::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::TRANSFORM);
}

bool Transform::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Position" << YAML::Value << m_position;
    emitter << YAML::Key << "Rotation" << YAML::Value << m_rotation;
    emitter << YAML::Key << "Scale" << YAML::Value << m_scale;
    return true;
}

bool Transform::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_position = data["Position"].as<glm::vec3>();
    m_rotation = data["Rotation"].as<glm::quat>();
    m_scale = data["Scale"].as<glm::vec3>();
    return true;
}

} // namespace scene
} // namespace aderite
