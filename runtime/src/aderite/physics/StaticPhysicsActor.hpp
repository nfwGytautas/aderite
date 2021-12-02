#pragma once

#include "aderite/physics/BasePhysicsActor.hpp"

namespace aderite {
namespace physics {

/**
 * @brief A static physics actor that is optimized for being set once and not moved
 */
class StaticPhysicsActor : public BasePhysicsActor {
public:
    StaticPhysicsActor();
    virtual ~StaticPhysicsActor();

    // Inherited via BasePhysicsActor
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

} // namespace physics
} // namespace aderite
