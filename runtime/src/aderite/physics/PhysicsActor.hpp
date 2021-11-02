#pragma once

#include <stack>

#include <PxRigidActor.h>
#include <glm/glm.hpp>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/components/Actors.hpp"
#include "aderite/scene/components/Transform.hpp"
#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace physics {

/**
 * @brief A base class for physics actors, could be static or dynamic
 */
class PhysicsActor : public io::ISerializable {
public:
    virtual ~PhysicsActor();

    /**
     * @brief Instantly moves the actor to the specified position
     * @param position New position of the actor
     */
    void moveActor(const glm::vec3& position);

    /**
     * @brief Instantly rotates the actor to the specified rotation
     * @param rotation New rotation of the actor
     */
    void rotateActor(const glm::quat& rotation);

    /**
     * @brief Synchronizes the physics actor with the ECS transform
     */
    void sync();

    /**
     * @brief Detaches all shapes attached to actor
     */
    void detachShapes();

    /**
     * @brief Function called when the actor enters a trigger zone
     * @param trigger Trigger zone
     */
    void onTriggerEnter(PhysicsActor* trigger);

    /**
     * @brief Function called when the actor leaves a trigger zone
     * @param trigger Trigger zone
     */
    void onTriggerLeave(PhysicsActor* trigger);

    /**
     * @brief Function called when the actor starts colliding with another actor
     * @param collision Actor that is being collided with
     */
    void onCollisionEnter(PhysicsActor* collision);

    /**
     * @brief Function called when the actor stops colliding with another actor
     * @param collision Actor that has stopped colliding with
     */
    void onCollisionLeave(PhysicsActor* collision);

private:
    scene::PhysicsCallbackComponent& ensureEventComponent();

private:
    friend class PhysicsScene;
    friend class Collider;

protected:
    scene::Entity m_entity = scene::Entity::null();
    physx::PxRigidActor* p_actor = nullptr;
};

} // namespace physics
} // namespace aderite
