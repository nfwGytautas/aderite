#pragma once

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/StaticPhysicsActor.hpp"
#include "aderite/rendering/Renderable.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Scenery class much like the visual is a immobile piece of the scene, the difference is that the scenery has a static physics actor
 * thus making it part of the physics simulation and allowing objects to collide with it
 */
class Scenery final : public physics::StaticPhysicsActor, public rendering::Renderable, public io::SerializableObject {
public:
    Scenery();
    virtual ~Scenery();

    // Inherited via StaticPhysicsActor
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    // Inherited via SerializableObject
    reflection::Type getType() const override;

private:
};

} // namespace scene
} // namespace aderite
