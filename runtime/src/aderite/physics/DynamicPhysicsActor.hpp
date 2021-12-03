#pragma once

#include "aderite/physics/BasePhysicsActor.hpp"

namespace aderite {
namespace physics {

/**
 * @brief A dynamic physics actor that can freely move and is part of various physics calculations as a free form body
 */
class DynamicPhysicsActor : public BasePhysicsActor {
public:
    DynamicPhysicsActor();
    virtual ~DynamicPhysicsActor();

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

    // Inherited via BasePhysicsActor
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

} // namespace physics
} // namespace aderite
