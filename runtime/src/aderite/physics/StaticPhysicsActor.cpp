#include "StaticPhysicsActor.hpp"

#include <physx/PxRigidStatic.h>
#include <physx/PxScene.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysicsController.hpp"

namespace aderite {
namespace physics {

StaticPhysicsActor::StaticPhysicsActor() {
    p_actor = ::aderite::Engine::getPhysicsController()->getPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(0)));
    p_actor->userData = this;
}

StaticPhysicsActor::~StaticPhysicsActor() {
    
}

bool StaticPhysicsActor::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "StaticPhysicsActor" << YAML::BeginMap;
    if (!BasePhysicsActor::serialize(serializer, emitter)) {
        return false;
    }
    emitter << YAML::EndMap;

    return true;
}

bool StaticPhysicsActor::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& actorNode = data["StaticPhysicsActor"];
    if (!actorNode || actorNode.IsNull()) {
        return false;
    }

    if (!BasePhysicsActor::deserialize(serializer, actorNode)) {
        return false;
    }

    return true;
}

} // namespace physics
} // namespace aderite
