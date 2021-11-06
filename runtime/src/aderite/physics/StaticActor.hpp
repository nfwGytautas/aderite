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

    // Inherited via PhysicsActor
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

} // namespace physics
} // namespace aderite
