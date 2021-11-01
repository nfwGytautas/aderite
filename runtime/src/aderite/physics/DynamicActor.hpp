#pragma once

#include "aderite/physics/PhysicsActor.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Dynamic physics actor, that is affected by gravity, can be applied with a force, etc.
 */
class DynamicActor final : public PhysicsActor {
public:
    DynamicActor();

    /**
     * @brief If value is true, then the actor will be made into a kinematic actor
     */
    void isKinematic(bool value);

    /**
     * @brief If value is true, then the actor will be affected by gravity
     */
    void hasGravity(bool value);

    /**
     * @brief Changes the mass of the dynamic actor
     * @param mass New mass of the actor
     */
    void setMass(float mass);

    // Inherited via PhysicsActor
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
private:
};

} // namespace physics
} // namespace aderite