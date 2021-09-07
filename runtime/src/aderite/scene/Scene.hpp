#pragma once

#include <vector>
#include <PxSimulationEventCallback.h>
#include <entt/entity/registry.hpp>
#include "aderite/Config.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderite/asset/Asset.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scene/ICamera.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/physics/Forward.hpp"


ADERITE_SCENE_NAMESPACE_BEGIN

/**
 * @brief Scene object, at any given time a single Scene can be active in aderite.
 * Scenes are guaranteed to be serializable. This includes all the possible resources that it uses
 * be it meshes, materials, etc. However these resources are loaded as trunks into the asset manager
 * the actual data is not loaded until needed.
*/
class Scene : public asset::Asset, public physx::PxSimulationEventCallback {
public:
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
	 * @brief Marks the asset as being used by the Scene
	 * @param asset Asset to use
	*/
	virtual void useAsset(asset::Asset* asset);

	/**
	 * @brief Removes the asset from the Scene
	 * @param asset Asset to remove
	*/
	virtual void removeAsset(asset::Asset* asset);

	/**
	 * @brief Returns the physics scene
	*/
	physx::PxScene* getPhysicsScene() const;

	// Inherited via asset_base
	virtual void prepareLoad() override;
	virtual bool isReadyToLoad() override;
	virtual void load() override;
	virtual void unload() override;
	virtual bool isPreparing() override;
	virtual bool isLoaded() override;
	virtual size_t hash() const override;

	virtual asset::AssetType type() const override;
	virtual bool isInGroup(asset::AssetGroup group) const override;

#if MIDDLEWARE_ENABLED == 1
	/**
	 * @brief Attach a middleware camera
	 * @param camera Middleware camera
	*/
	void attachMiddlewareCamera(interfaces::ICamera* camera) {
		m_middlewareCamera = camera;
	}

	/**
	 * @brief Returns the attached middleware camera or nullptr
	*/
	interfaces::ICamera* getMiddlewareCamera() const {
		return m_middlewareCamera;
	}

private:
	interfaces::ICamera* m_middlewareCamera = nullptr;
public:
#endif
protected:
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	Scene(const std::string& name);

	friend class Entity;
	friend class SceneManager;
	friend class ::aderite::asset::AssetManager;
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
private:
	// Assets that the Scene uses
	std::vector<asset::Asset*> m_assets;
	entt::registry m_registry;
	physx::PxScene* m_physicsScene = nullptr;

	// Properties
	asset::TextureAsset* m_skybox = nullptr;
	glm::vec3 m_skyColor = { 0.53f, 0.81f, 0.98f }; // Only used if m_skybox is nullptr
};

ADERITE_SCENE_NAMESPACE_END
