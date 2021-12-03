#include "StaticPhysicsRegion.hpp"

namespace aderite {
namespace scene {

StaticPhysicsRegion::StaticPhysicsRegion() {}

StaticPhysicsRegion::~StaticPhysicsRegion() {}

bool StaticPhysicsRegion::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Physics
    if (!StaticPhysicsActor::serialize(serializer, emitter)) {
        return false;
    }

    return true;
}

bool StaticPhysicsRegion::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Physics
    if (!StaticPhysicsActor::deserialize(serializer, data)) {
        return false;
    }

    return true;
}

reflection::Type StaticPhysicsRegion::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::STATIC_PHYSICS_REGION);
}

} // namespace scene
} // namespace aderite
