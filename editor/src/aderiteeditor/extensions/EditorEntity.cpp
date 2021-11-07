#include "EditorEntity.hpp"

#include "aderite/utility/Random.hpp"

namespace aderite {
namespace scene {

EditorEntity::EditorEntity() : m_name(utility::generateString(16)) {}

std::string EditorEntity::getName() const {
    return m_name;
}

void EditorEntity::setName(const std::string& name) {
    // TODO: Check if unique
    m_name = name;
}

bool EditorEntity::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!Entity::serialize(serializer, emitter)) {
        return false;
    }

    emitter << YAML::Key << "Name" << YAML::Value << m_name;

    return true;
}

bool EditorEntity::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!Entity::deserialize(serializer, data)) {
        return false;
    }

    m_name = data["Name"].as<std::string>();

    return true;
}

} // namespace scene
} // namespace aderite
