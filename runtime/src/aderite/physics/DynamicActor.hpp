#pragma once

#include "aderite/physics/PhysicsActor.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Dynamic physics actor, that is affected by gravity, can be applied with a force, etc.
 */
class DynamicActor : public PhysicsActor {
public:
    DynamicActor();
    virtual ~DynamicActor() {}

    /**
     * @brief If value is true, then the actor will be made into a kinematic actor
     */
    void setKinematic(bool value);

    /**
     * @brief Returns true if the actor is kinematic, false otherwise
    */
    bool getKinematic() const;

    /**
     * @brief If value is true, then the actor will be affected by gravity
     */
    void setGravity(bool value);

    /**
     * @brief Returns true if the actor is affected by gravity, false otherwise
    */
    bool getGravity() const;

    /**
     * @brief Changes the mass of the dynamic actor
     * @param mass New mass of the actor
     */
    void setMass(float mass);

    /**
     * @brief Returns the mass of the actor
    */
    float getMass() const;

    // Inherited via PhysicsActor
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
private:
};

} // namespace physics
} // namespace aderite