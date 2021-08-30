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
	 * @brief Completely resets the physics scene
	*/
	void reset();

	/**
	 * @brief Creates new collider list and returns it
	*/
	ColliderList* newColliderList();

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

	/**
	 * @brief Creates a rigid body from a component and adds it to the physics scene
	 * @param rbody Component to create from
	 * @param colliders Object colliders
	 * @param transform Transform component of the entity
	*/
	void createRigidbody(
		scene::components::RigidbodyComponent& rbody, 
		const scene::components::CollidersComponent& colliders, 
		const scene::components::TransformComponent& transform);

	/**
	 * @brief Creates a static body from a component and adds it to the physics scene
	 * @param rbody Component to create from
	 * @param colliders Object colliders
	 * @param transform Transform component of the entity
	*/
	void createStaticbody(
		scene::components::RigidbodyComponent& rbody,
		const scene::components::CollidersComponent& colliders,
		const scene::components::TransformComponent& transform);

	/**
	 * @brief Sets up a rigid body from it's components
	 * @param actor The PhysX actor
	 * @param rbody Body settings
	 * @param colliders Object colliders
	 * @param transform Transform component of the entity
	*/
	void setupBody(
		physx::PxRigidActor* actor,
		const scene::components::RigidbodyComponent& rbody,
		const scene::components::CollidersComponent& colliders,
		const scene::components::TransformComponent& transform);

	/**
	 * @brief Creates a trigger from component information
	 * @param colliders Collider information of the entity
	 * @param transform Transformation information of the entity
	*/
	void createTrigger(
		const scene::components::CollidersComponent& colliders,
		const scene::components::TransformComponent& transform);

	/**
	 * @brief Syncs PhysX actor to ECS entity state
	 * @param actor Actor to sync
	 * @param transform Entity transform to sync with
	*/
	void syncActor(
		const scene::components::RigidbodyComponent& rbody,
		const scene::components::TransformComponent& transform);

	/**
	 * @brief Syncs ECS entity state to PhysX actor state
	 * @param rbody Actor to sync
	 * @param transform Entity transform to sync to
	*/
	void syncTransform(
		const scene::components::RigidbodyComponent& rbody,
		scene::components::TransformComponent& transform);
private:
	physx::PxFoundation* m_foundation = nullptr;
	physx::PxPhysics* m_physics = nullptr;
	physx::PxCooking* m_cooking = nullptr;
	physx::PxDefaultCpuDispatcher* m_dispatcher = nullptr;
	physx::PxScene* m_scene = nullptr;
	physx::PxMaterial* m_defaultMaterial = nullptr;
	physx::PxPvd* m_pvd = nullptr;

	std::vector<physx::PxShape*> m_triggers;
	std::vector<ColliderList*> m_colliderLists;

	bool m_recordMemoryAllocations = true;
	bool m_isCreating = false;
	size_t m_numThreads = 2;

	float m_defaultStaticFriction = 0.5f;
	float m_defaultDynamicFriction = 0.5f;
	float m_defaultRestitution = 0.6f;
	float m_defaultDensity = 10.0f;
	float m_defaultAngularDamping = 0.5f;
};

ADERITE_PHYSICS_NAMESPACE_END
