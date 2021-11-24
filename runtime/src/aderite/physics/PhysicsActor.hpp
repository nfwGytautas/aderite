#pragma once

#include <stack>

#include <PxRigidActor.h>
#include <glm/glm.hpp>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Forward.hpp"

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
    const std::vector<physics::Collider*>& getColliders() const;

    /**
     * @brief Sets the entity of this actor
     * @param entity Entity instance
     */
    void setEntity(scene::Entity* entity);

    /**
     * @brief Returns the entity of this actor
     */
    scene::Entity* getEntity() const;

    /**
     * @brief Instantly moves the actor to the specified position
     * @param position New position of the actor
     */
    void moveActor(const glm::vec3& position) const;

    /**
     * @brief Instantly rotates the actor to the specified rotation
     * @param rotation New rotation of the actor
     */
    void rotateActor(const glm::quat& rotation) const;

    /**
     * @brief Synchronizes the physics actor with it's entity
     */
    void sync() const;

    /**
     * @brief Function called when the actor enters a trigger zone
     * @param trigger Trigger zone
     */
    void onTriggerEnter(PhysicsActor* trigger) const;

    /**
     * @brief Function called when the actor leaves a trigger zone
     * @param trigger Trigger zone
     */
    void onTriggerLeave(PhysicsActor* trigger) const;

    /**
     * @brief Function called when the actor starts colliding with another actor
     * @param collision Actor that is being collided with
     */
    void onCollisionEnter(PhysicsActor* collision) const;

    /**
     * @brief Function called when the actor stops colliding with another actor
     * @param collision Actor that has stopped colliding with
     */
    void onCollisionLeave(PhysicsActor* collision) const;

    /**
     * @brief Creates a replica of this physics actor
     * @return PhysicsActor instance
     */
    virtual PhysicsActor* clone() const = 0;

    // Inherited via ISerializable
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

protected:
    PhysicsActor();

    /**
     * @brief Clones this physics actor into the specified actor
     * @param actor Actor to clone into
     */
    void cloneInto(PhysicsActor* actor) const;

private:
    friend class PhysicsScene;

protected:
    physx::PxRigidActor* p_actor = nullptr;

private:
    scene::Entity* m_entity = nullptr;
    std::vector<physics::Collider*> m_colliders;
};

} // namespace physics
} // namespace aderite
