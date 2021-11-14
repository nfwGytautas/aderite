#pragma once

#include "aderite/physics/PhysicsActor.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Static physics actor, which ideally should never move
 */
class StaticActor final : public PhysicsActor {
public:
    StaticActor();
    ~StaticActor() {}

    // Inherited via PhysicsActor
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

} // namespace physics
} // namespace aderite
