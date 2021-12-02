#include "Visual.hpp"

#include "aderite/utility/Random.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace scene {

Visual::Visual() {}

reflection::Type Visual::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::VISUAL);
}

bool Visual::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Transform
    emitter << YAML::Key << "Transform" << YAML::BeginMap;
    emitter << YAML::Key << "Position" << YAML::Value << m_position;
    emitter << YAML::Key << "Rotation" << YAML::Value << m_rotation;
    emitter << YAML::Key << "Scale" << YAML::Value << m_scale;
    emitter << YAML::EndMap;

    // Rendering
    if (!Renderable::serialize(serializer, emitter)) {
        return false;
    }

    return true;
}

bool Visual::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Transform
    const YAML::Node& transformNode = data["Transform"];
    m_position = transformNode["Position"].as<glm::vec3>();
    m_rotation = transformNode["Rotation"].as<glm::quat>();
    m_scale = transformNode["Scale"].as<glm::vec3>();

    // Rendering
    if (!Renderable::deserialize(serializer, data)) {
        return false;
    }

    return true;
}

const glm::vec3& Visual::getPosition() const {
    return m_position;
}

const glm::quat& Visual::getRotation() const {
    return m_rotation;
}

const glm::vec3& Visual::getScale() const {
    return m_scale;
}

void Visual::setPosition(const glm::vec3& position) {
    m_position = position;
}

void Visual::setRotation(const glm::quat& rotation) {
    m_rotation = rotation;
}

void Visual::setScale(const glm::vec3& scale) {
    m_scale = scale;
}

} // namespace scene
} // namespace aderite
