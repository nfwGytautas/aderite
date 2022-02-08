#include "TransformProvider.hpp"

#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace scene {

bool TransformProvider::wasModified() const {
    return m_wasModified;
}

void TransformProvider::resetModifiedFlag() {
    m_wasModified = false;
}

const glm::vec3& TransformProvider::getPosition() const {
    return m_position;
}

const glm::quat& TransformProvider::getRotation() const {
    return m_rotation;
}

const glm::vec3& TransformProvider::getScale() const {
    return m_scale;
}

void TransformProvider::setPosition(const glm::vec3& position) {
    m_wasModified = true;
    m_position = position;
}

void TransformProvider::setRotation(const glm::quat& rotation) {
    m_wasModified = true;
    m_rotation = rotation;
}

void TransformProvider::setScale(const glm::vec3& scale) {
    m_wasModified = true;
    m_scale = scale;
}

bool TransformProvider::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Transform
    emitter << YAML::Key << "Transform" << YAML::BeginMap;
    emitter << YAML::Key << "Position" << YAML::Value << this->getPosition();
    emitter << YAML::Key << "Rotation" << YAML::Value << this->getRotation();
    emitter << YAML::Key << "Scale" << YAML::Value << this->getScale();
    emitter << YAML::EndMap;

    return true;
}
bool TransformProvider::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Transform
    const YAML::Node& transformNode = data["Transform"];
    if (!transformNode || transformNode.IsNull()) {
        return false;
    }

    this->setPosition(transformNode["Position"].as<glm::vec3>());
    this->setRotation(transformNode["Rotation"].as<glm::quat>());
    this->setScale(transformNode["Scale"].as<glm::vec3>());

    return true;
}

TransformProvider& TransformProvider::operator=(const TransformProvider& other) {
    m_position = other.m_position;
    m_rotation = other.m_rotation;
    m_scale = other.m_scale;
    return *this;
}

} // namespace scene
} // namespace aderite
