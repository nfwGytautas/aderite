#pragma once

#include <stack>

#include <PxRigidActor.h>
#include <glm/glm.hpp>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Forward.hpp"
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
     * @brief Add collider to the physics actor
     * @param collider Collider to add
     */
    void addCollider(physics::Collider* collider);

    /**
     * @brief Remove collider from the actor
     * @param collider Collider to remove
     */
    void removeCollider(physics::Collider* collider);

    /**
     * @brief Transfers colliders from this actor to the specified actor
     * @param actor Actor to transfer to
    */
    void transferColliders(PhysicsActor* actor);

    /**
     * @brief Returns the list of colliders for this actor
     */
    const std::vector<physics::Collider*>& getColliders() const {
        return m_colliders;
    }

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
     * @brief Synchronizes the physics actor with the transform
     * @param transform Transform where to store result
     */
    void sync(scene::Transform* transform) const;

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

    // Inherited via ISerializable
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    friend class PhysicsScene;

protected:
    physx::PxRigidActor* p_actor = nullptr;

private:
    std::vector<physics::Collider*> m_colliders;
};

} // namespace physics
} // namespace aderite
