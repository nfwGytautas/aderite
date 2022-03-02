#pragma once

#include <PxSimulationEventCallback.h>
#include <glm/glm.hpp>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/physics/PhysicsSceneQuery.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Physics scene for aderite
 */
class PhysicsScene : public physx::PxSimulationEventCallback, public io::ISerializable {
public:
    PhysicsScene();
    virtual ~PhysicsScene();

    /**
     * @brief Physics update
     * @param step Fixed step size
     */
    void simulate(float step) const;

    /**
     * @brief Add actor to the scene
     * @param actor Actor to add
     */
    void addActor(PhysXActor* actor);

    /**
     * @brief Sends queued physics events to specified user callbacks
     */
    void sendEvents();

    /**
     * @brief Do a first hit raycast
     * @param result RaycastHit object where to store result
     * @param from Point where to raycast from
     * @param direction Direction which to raycast in
     * @param maxDistance Maximum distance of the ray
     * @return True if there was a hit, false otherwise
     */
    bool raycastSingle(RaycastResult& result, const glm::vec3& from, const glm::vec3& direction, float maxDistance);

protected:
    // Inherited via ISerializable
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    // Inherited via PxSimulationEventCallback
    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs) override;
    void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override {}
    void onWake(physx::PxActor**, physx::PxU32) override {}
    void onSleep(physx::PxActor**, physx::PxU32) override {}
    void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) override {}

private:
    physx::PxScene* m_scene = nullptr;
    PhysicsEventList* m_events = nullptr;
};

} // namespace physics
} // namespace aderite