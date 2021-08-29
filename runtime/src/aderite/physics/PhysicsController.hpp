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
	 * @brief Creates and attaches a rigid body to an entity
	 * @param on Entity the rigidbody is attached to
	*/
	void attachRigidBody(scene::Entity on);

	/**
	 * @brief Creates a new collider list and returns it
	*/
	ColliderList* beginNewColliderList();

	/**
	 * @brief Returns the PhysX physics object instance
	*/
	physx::PxPhysics* getPhysics();

	/**
	 * @brief Returns the default physics material instance
	*/
	physx::PxMaterial* getDefaultMaterial();
private:
	PhysicsController() {}
	friend class Engine;

private:
	physx::PxFoundation* m_foundation = nullptr;
	physx::PxPhysics* m_physics = nullptr;
	physx::PxCooking* m_cooking = nullptr;
	physx::PxDefaultCpuDispatcher* m_dispatcher = nullptr;
	physx::PxScene* m_scene = nullptr;
	physx::PxMaterial* m_defaultMaterial = nullptr;

	std::vector<Rigidbody*> m_bodies;
	std::vector<ColliderList*> m_colliderLists;

	bool m_recordMemoryAllocations = true;
	size_t m_numThreads = 2;

	float m_defaultStaticFriction = 0.5f;
	float m_defaultDynamicFriction = 0.5f;
	float m_defaultRestitution = 0.6f;
	float m_defaultDensity = 10.0f;
	float m_defaultAngularDamping = 0.5f;
};

ADERITE_PHYSICS_NAMESPACE_END
