#pragma once

#include "aderite/physics/DynamicPhysicsActor.hpp"
#include "aderite/rendering/Renderable.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Entity class is used to denote an object that acts in the scene, be it a NPC or a player character, maybe some other physics
 * object, basically this is a mobile object
 */
class Entity final : public physics::DynamicPhysicsActor, public rendering::Renderable, public io::SerializableObject {
public:
    Entity();
    virtual ~Entity();

    // Inherited via DynamicPhysicsActor
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    // Inherited via SerializableObject
    virtual reflection::Type getType() const override;

private:
};

} // namespace scene
} // namespace aderite
