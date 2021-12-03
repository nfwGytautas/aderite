#include "Entity.hpp"

namespace aderite {
namespace scene {

Entity::Entity() {}

Entity::~Entity() {}

bool Entity::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Physics
    if (!DynamicPhysicsActor::serialize(serializer, emitter)) {
        return false;
    }

    // Rendering
    if (!Renderable::serialize(serializer, emitter)) {
        return false;
    }

    return true;
}

bool Entity::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Physics
    if (!DynamicPhysicsActor::deserialize(serializer, data)) {
        return false;
    }

    // Rendering
    if (!Renderable::deserialize(serializer, data)) {
        return false;
    }

    return true;
}

reflection::Type Entity::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::ENTITY);
}

} // namespace scene
}
