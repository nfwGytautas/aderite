#pragma once

#include "aderite/asset/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/StaticPhysicsActor.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/scene/ITransformProvider.h"

namespace aderite {
namespace scene {

/**
 * @brief Scenery class much like the visual is a immobile piece of the scene, the difference is that the scenery has a static physics actor
 * thus making it part of the physics simulation and allowing objects to collide with it
 */
class Scenery final : public physics::StaticPhysicsActor, public rendering::Renderable, public io::NamedObject {
public:
    Scenery();
    virtual ~Scenery();

    // Inherited via StaticPhysicsActor
    virtual reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
};

} // namespace scene
} // namespace aderite
