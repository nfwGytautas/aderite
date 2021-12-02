#include "Scenery.hpp"

namespace aderite {
namespace scene {

Scenery::Scenery() {}

Scenery::~Scenery() {}

reflection::Type Scenery::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::SCENERY);
}

bool Scenery::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Physics
    if (!StaticPhysicsActor::serialize(serializer, emitter)) {
        return false;
    }

    // Rendering
    if (!Renderable::serialize(serializer, emitter)) {
        return false;
    }

    return true;
}

bool Scenery::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Physics
    if (!StaticPhysicsActor::deserialize(serializer, data)) {
        return false;
    }

    // Rendering
    if (!Renderable::deserialize(serializer, data)) {
        return false;
    }

    return true;
}

} // namespace scene
} // namespace aderite
