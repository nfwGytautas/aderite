#pragma once

#include <vector>
#include <PxSimulationEventCallback.h>
#include <entt/entity/registry.hpp>
#include "aderite/Config.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/rendering/Forward.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Scene object, at any given time a single Scene can be active in aderite.
 * Scenes are guaranteed to be serializable. This includes all the possible resources that it uses
 * be it meshes, materials, etc. However these resources are loaded as trunks into the asset manager
 * the actual data is not loaded until needed.
*/
class Scene : public io::SerializableObject, public physx::PxSimulationEventCallback {
public:
	Scene();
	virtual ~Scene();

	/**
	 * @brief Returns entt registry
	*/
	entt::registry& getEntityRegistry() {
		return m_registry;
	}

	/**
	 * @brief Update scene
	 * @param delta Delta time between frames
	*/
	void update(float delta);

	/**
	 * @brief Physics update
	 * @param step Fixed step size
	*/
	void fixedUpdate(float step);

	/**
	 * @brief Create Entity with a MetaComponent component
	 * @return Entity instance
	*/
	Entity createEntity(const components::MetaComponent& MetaComponent);

	/**
	 * @brief Destroy an Entity
	*/
	void destroyEntity(Entity entity);

	/**
	 * @brief Returns the physics scene
	*/
	physx::PxScene* getPhysicsScene() const;

	/**
	 * @brief Returns the pipeline of this scene
	*/
	rendering::Pipeline* getPipeline() const;

	/**
	 * @brief Sets the pipeline of the scene
	 * @param pipeline New pipeline
	*/
	void setPipeline(rendering::Pipeline* pipeline);

	// Inherited via SerializableObject
	virtual io::SerializableType getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
private:
	/**
	 * @brief Function invoked right after a component was added to entity
	*/
	template<typename T>
	void onComponentAdded(Entity entity, T& component);

	/**
	 * @brief Function invoked right before a component is removed from entity
	*/
	template<typename T>
	void onComponentRemoved(Entity entity, T& component);

	// Inherited via PxSimulationEventCallback
	virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
	virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs) override;
	virtual void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {}
	virtual void onWake(physx::PxActor**, physx::PxU32) {}
	virtual void onSleep(physx::PxActor**, physx::PxU32) {}
	virtual void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) {}

	/**
	 * @brief Syncs physics actor to ECS entity state
	 * @param actor Actor to sync
	 * @param colliders Colliders component of the entity
	 * @param transform Entity transform to sync with
	*/
	void syncActorToEcs(
		physx::PxRigidActor* actor,
		const scene::components::CollidersComponent& colliders,
		const scene::components::TransformComponent& transform);

	/**
	 * @brief Syncs ECS state to physics
	*/
	void syncEcsToPhysics();

	/**
	 * @brief Syncs physics state to ECS
	*/
	void syncPhysicsToEcs();

	friend class Entity;
	friend class SceneManager;
private:
	entt::registry m_registry;
	physx::PxScene* m_physicsScene = nullptr;
	rendering::Pipeline* m_pipeline = nullptr;
};

}
}
