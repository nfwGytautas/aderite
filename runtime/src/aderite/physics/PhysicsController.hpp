#pragma once

#include <vector>
#include <PxPhysics.h>
#include <cooking/PxCooking.h>
#include <extensions/PxExtensionsAPI.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Entity.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

/**
 * @brief Class used to handle all physics related functionality for aderite
*/
class PhysicsController {
public:
	/**
	 * @brief Initializes the window manager
	*/
	bool init();

	/**
	 * @brief Shutdown the window manager
	*/
	void shutdown();

	/**
	 * @brief Function invoked by the engine when it's time to update physics
	 * @param delta Delta between two frames
	*/
	void update(float delta);

	/**
	 * @brief Creates a static rigidbody and returns it
	*/
	physx::PxRigidStatic* createStaticBody();

	/**
	 * @brief Creates a dynamic rigidbody and returns it
	*/
	physx::PxRigidDynamic* createDynamicBody();

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
	static physx::PxFilterFlags filterShader(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);
private:
	PhysicsController() {}
	friend class Engine;
private:
	physx::PxFoundation* m_foundation = nullptr;
	physx::PxPhysics* m_physics = nullptr;
	physx::PxCooking* m_cooking = nullptr;
	physx::PxDefaultCpuDispatcher* m_dispatcher = nullptr;
	physx::PxMaterial* m_defaultMaterial = nullptr;
	physx::PxPvd* m_pvd = nullptr;

	bool m_recordMemoryAllocations = true;
	size_t m_numThreads = 2;

	float m_defaultStaticFriction = 0.5f;
	float m_defaultDynamicFriction = 0.5f;
	float m_defaultRestitution = 0.6f;
	float m_defaultDensity = 10.0f;
	float m_defaultAngularDamping = 0.5f;
};

ADERITE_PHYSICS_NAMESPACE_END