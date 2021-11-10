#pragma once

#include <vector>

#include <PxPhysics.h>
#include <cooking/PxCooking.h>
#include <extensions/PxExtensionsAPI.h>

#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
class Engine;

namespace physics {
/**
 * @brief Class used to handle all physics related functionality for aderite
 */
class PhysicsController {
    static constexpr float c_FixedUpdateWindow = 0.02f;

public:
    /**
     * @brief Initializes the physics controller
     */
    bool init();

    /**
     * @brief Shutdown the physics controller
     */
    void shutdown();

    /**
     * @brief Function invoked by the engine when it's time to update physics
     * @param delta Delta between two frames
     */
    void update(float delta);

    /**
     * @brief Returns the physics event list instance
     */
    PhysicsEventList* getEventList() const;

    /**
     * @brief Returns the PhysX physics object instance
     */
    physx::PxPhysics* getPhysics();

    /**
     * @brief Returns the PhysX CPU dispatcher
     */
    physx::PxCpuDispatcher* getDispatcher();

    /**
     * @brief Returns the default physics material instance
     */
    physx::PxMaterial* getDefaultMaterial();

    /**
     * @brief Filter shader of the physics controller
     * @param attributes0 Attributes of the first actor
     * @param filterData0 Filter data of the first actor
     * @param attributes1 Attributes of the second actor
     * @param filterData1 Filter data of the second actor
     * @param pairFlags Pair flags that are returned
     * @param constantBlock -
     * @param constantBlockSize -
     * @return Filter flag, to be suppressed or not
     */
    static physx::PxFilterFlags filterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
                                             physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
                                             physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

private:
    /**
     * @brief Synchronize changes between physics world and ECS
     */
    void syncChanges();

    /**
     * @brief Removes detached actors from pool
     */
    void removeDetached();

private:
    PhysicsController() {}
    friend Engine;

private:
    physx::PxFoundation* m_foundation = nullptr;
    physx::PxPhysics* m_physics = nullptr;
    physx::PxCooking* m_cooking = nullptr;
    physx::PxDefaultCpuDispatcher* m_dispatcher = nullptr;
    physx::PxMaterial* m_defaultMaterial = nullptr;
    physx::PxPvd* m_pvd = nullptr;

    float m_accumulator = 0.0f;
    PhysicsEventList* m_events = nullptr;

    bool m_recordMemoryAllocations = true;
    size_t m_numThreads = 2;

    float m_defaultStaticFriction = 0.5f;
    float m_defaultDynamicFriction = 0.5f;
    float m_defaultRestitution = 0.6f;
    float m_defaultDensity = 10.0f;
    float m_defaultAngularDamping = 0.5f;
};

} // namespace physics
} // namespace aderite
