#pragma once

#include <PxSimulationEventCallback.h>

#include "aderite/io/SerializableObject.hpp"

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
     * @brief Initialize the physics scene
     * @param controller Physics controller
     */
    void initialize(physics::PhysicsController* controller);

    /**
     * @brief Adds a physics actor to the scene
     * @param actor Actor to add
     */
    void addActor(PhysicsActor* actor);

    /**
     * @brief Returns true if the physics scene was initialized, false otherwise
     */
    bool initialized() const;

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