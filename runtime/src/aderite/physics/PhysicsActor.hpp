#pragma once

#include <stack>

#include <PxRigidActor.h>
#include <glm/glm.hpp>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Collision.hpp"
#include "aderite/physics/Forward.hpp"
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
     * @param transform Transform component where to store result
     */
    void sync(scene::TransformComponent& transform) const;

    /**
     * @brief Marks the actor to be removed on the next physics update
     */
    void markForRemove();

    /**
     * @brief Returns true if the actor has been marked for removal
     */
    bool isMarkedForRemoval() const;

    /**
     * @brief Detaches the actor from it's scene
     */
    void detach();

    /**
     * @brief Sets the notifier for the actor, the notifier will be deleted together with the actor
     * @param notifier New notifier for the actor
     */
    void setNotifier(EventNotifier* notifier);

    /**
     * @brief Returns the event notifier for this actor
     */
    EventNotifier* getNotifier() const;

    /**
     * @brief Sets the collider list of the actor, any previous colliders are detached
     * @param colliders Collider list
     */
    void setColliders(ColliderList* colliders);

    /**
     * @brief Updates the collider list of the actor
     */
    void updateColliders();

private:
    friend class PhysicsScene;

protected:
    physx::PxRigidActor* p_actor = nullptr;
    EventNotifier* m_notifier = nullptr;
    ColliderList* m_colliders = nullptr;
    bool m_marked = false;
};

} // namespace physics
} // namespace aderite
