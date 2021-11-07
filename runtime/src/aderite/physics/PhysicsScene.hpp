#pragma once

#include <PxSimulationEventCallback.h>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Entity.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Physics scene for aderite
 */
class PhysicsScene final : public physx::PxSimulationEventCallback, public io::ISerializable {
public:
    PhysicsScene();
    ~PhysicsScene();

    /**
     * @brief Physics update
     * @param step Fixed step size
     */
    void simulate(float step);

    /**
     * @brief Add a physics actor at the specified transform
     * @param actor Actor to add
     * @param initialTransform Optional initial transform
     */
    void addActor(physics::PhysicsActor* actor, const scene::Transform* initialTransform = nullptr);

    /**
     * @brief Detach the specified actor from physics scene
     * @param actor Actor to detach
     */
    void detachActor(physics::PhysicsActor* actor);

    /**
     * @brief Returns a vector of all actors in this scene
     */
    const std::vector<PhysicsActor*>& getActors() const {
        return m_actors;
    }

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

    std::vector<PhysicsActor*> m_actors;

    // Inherited via ISerializable
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

} // namespace physics
} // namespace aderite