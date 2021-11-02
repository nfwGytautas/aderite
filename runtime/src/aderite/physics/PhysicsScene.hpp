#pragma once

#include <PxSimulationEventCallback.h>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/components/Transform.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Physics scene for aderite
 */
class PhysicsScene final : public physx::PxSimulationEventCallback {
public:
    PhysicsScene();
    ~PhysicsScene();

    /**
     * @brief Physics update
     * @param step Fixed step size
     */
    void simulate(float step);

    /**
     * @brief Creates a static rigidbody and returns it
     * @param e Entity to create for
     * @param transform Initial transform of the actor
     */
    physics::PhysicsActor* createStaticBody(scene::Entity e, const scene::TransformComponent& transform);

    /**
     * @brief Creates a dynamic rigidbody and returns it
     * @param e Entity to create for
     * @param transform Initial transform of the actor
     */
    physics::PhysicsActor* createDynamicBody(scene::Entity e, const scene::TransformComponent& transform);

    /**
     * @brief Detach the specified actor from physics scene
     * @param actor Actor to detach
     */
    void detachActor(physics::PhysicsActor* actor);

private:
    // Inherited via PxSimulationEventCallback
    virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
    virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs) override;
    virtual void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {}
    virtual void onWake(physx::PxActor**, physx::PxU32) {}
    virtual void onSleep(physx::PxActor**, physx::PxU32) {}
    virtual void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) {}

private:
    physx::PxScene* m_scene = nullptr;
};

} // namespace physics
} // namespace aderite