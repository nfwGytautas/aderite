#pragma once

#include <vector>
#include <entt/entity/registry.hpp>
#include "aderite/utility/Macros.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderite/asset/Asset.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scene/components/Components.hpp"


ADERITE_SCENE_NAMESPACE_BEGIN

/**
 * @brief Scene object, at any given time a single Scene can be active in aderite.
 * Scenes are guaranteed to be serializable. This includes all the possible resources that it uses
 * be it meshes, materials, etc. However these resources are loaded as trunks into the asset manager
 * the actual data is not loaded until needed.
*/
class Scene : public asset::Asset {
public:
	/**
	 * @brief Returns entt registry
	*/
	entt::registry& getEntityRegistry() {
		return m_registry;
	}

	/**
	 * @brief Create Entity with a MetaComponent component
	 * @return Entity instance
	*/
	Entity createEntity(const components::MetaComponent& MetaComponent);

	/**
	 * @brief Destroy an Entity
	*/
	void destroyEntity(Entity Entity);

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

	// Inherited via asset_base
	virtual void prepareLoad() override;
	virtual bool isReadyToLoad() override;
	virtual void load() override;
	virtual void unload() override;
	virtual bool isPreparing() override;
	virtual bool isLoaded() override;

	virtual asset::AssetType type() const override;
	virtual bool isInGroup(asset::AssetGroup group) const override;
protected:
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	Scene(const std::string& name)
		: Asset(name)
	{}

	friend class SceneManager;
	friend class ::aderite::asset::AssetManager;
private:
	// Assets that the Scene uses
	std::vector<asset::Asset*> m_assets;
	entt::registry m_registry;
};

ADERITE_SCENE_NAMESPACE_END
