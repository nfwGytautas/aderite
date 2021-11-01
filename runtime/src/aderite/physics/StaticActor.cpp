#include "StaticActor.hpp"

#include <PxRigidStatic.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"

namespace aderite {
namespace physics {

StaticActor::StaticActor() {
    p_actor = ::aderite::Engine::getPhysicsController()->createStaticBody();
    p_actor->userData = this;
}

reflection::Type StaticActor::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::STATIC_ACTOR);
}

bool StaticActor::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    return true;
}

bool StaticActor::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    return true;
}

} // namespace physics
} // namespace aderite
