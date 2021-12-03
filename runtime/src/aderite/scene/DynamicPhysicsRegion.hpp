#pragma once

#include "aderite/physics/DynamicPhysicsActor.hpp"
#include "aderite/rendering/Renderable.hpp"

namespace aderite {
namespace scene {

/**
 * @brief DynamicPhysicsRegion class is used to create a mobile invisible physics collider/trigger region, for example a moving speedboost
 */
class DynamicPhysicsRegion final : public physics::DynamicPhysicsActor, public io::SerializableObject {
public:
    DynamicPhysicsRegion();
    virtual ~DynamicPhysicsRegion();

    // Inherited via DynamicPhysicsActor
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    // Inherited via SerializableObject
    virtual reflection::Type getType() const override;

private:
};

} // namespace scene
} // namespace aderite
