#include "TransformProvider.hpp"

#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace scene {

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
    this->setPosition(transformNode["Position"].as<glm::vec3>());
    this->setRotation(transformNode["Rotation"].as<glm::quat>());
    this->setScale(transformNode["Scale"].as<glm::vec3>());

    return true;
}

} // namespace scene
} // namespace aderite
