#include "EntitySelector.hpp"

namespace aderite {
namespace scene {

void EntitySelector::setName(const std::string& name) {
    m_name = name;
}

void EntitySelector::setScene(scene::Scene* scene) {
    p_scene = scene;
    this->regenerate();
}

const std::string& EntitySelector::getName() const {
    return m_name;
}

const scene::Scene* EntitySelector::getScene() const {
    return p_scene;
}

bool EntitySelector::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Name" << YAML::Value << m_name;

    return true;
}

bool EntitySelector::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_name = data["Name"].as<std::string>();

    return true;
}

} // namespace scene
} // namespace aderite
