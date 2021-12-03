#pragma once

#include "aderite/asset/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/StaticPhysicsActor.hpp"

namespace aderite {
namespace scene {

/**
 * @brief StaticPhysicsRegion class is used to create an immobile invisible physics collider/trigger region, for example a checkpoint
 */
class StaticPhysicsRegion final : public physics::StaticPhysicsActor, public io::SerializableObject {
public:
    StaticPhysicsRegion();
    virtual ~StaticPhysicsRegion();

    // Inherited via StaticPhysicsActor
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    // Inherited via SerializableObject
    reflection::Type getType() const override;

private:
};

} // namespace scene
} // namespace aderite
