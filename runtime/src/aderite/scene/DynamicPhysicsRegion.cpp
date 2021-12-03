#include "DynamicPhysicsRegion.hpp"

namespace aderite {
namespace scene {

DynamicPhysicsRegion::DynamicPhysicsRegion() {}

DynamicPhysicsRegion::~DynamicPhysicsRegion() {}

bool DynamicPhysicsRegion::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Physics
    if (!DynamicPhysicsActor::serialize(serializer, emitter)) {
        return false;
    }

    return true;
}

bool DynamicPhysicsRegion::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Physics
    if (!DynamicPhysicsActor::deserialize(serializer, data)) {
        return false;
    }

    return true;
}

reflection::Type DynamicPhysicsRegion::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::DYNAMIC_PHYSICS_REGION);
}

} // namespace scene
} // namespace aderite
