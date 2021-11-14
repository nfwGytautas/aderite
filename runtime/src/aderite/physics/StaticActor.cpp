#include "StaticActor.hpp"

#include <PxRigidStatic.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"

namespace aderite {
namespace physics {

StaticActor::StaticActor() {
    p_actor = ::aderite::Engine::getPhysicsController()->getPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(0)));
    p_actor->userData = this;
}

reflection::Type StaticActor::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::STATIC_ACTOR);
}

bool StaticActor::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!PhysicsActor::serialize(serializer, emitter)) {
        return false;
    }
    return true;
}

bool StaticActor::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!PhysicsActor::deserialize(serializer, data)) {
        return false;
    }
    return true;
}

} // namespace physics
} // namespace aderite
