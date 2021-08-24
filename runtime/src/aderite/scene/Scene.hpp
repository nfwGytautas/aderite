#pragma once

#include <vector>
#include <entt/entity/registry.hpp>
#include "aderite/utility/macros.hpp"
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
	entt::registry& get_entity_registry() {
		return m_registry;
	}

	/**
	 * @brief Create Entity with a MetaComponent component
	 * @return Entity instance
	*/
	Entity create_entity(const components::MetaComponent& MetaComponent);

	/**
	 * @brief Destroy an Entity
	*/
	void destroy_entity(Entity Entity);

	/**
	 * @brief Marks the asset as being used by the Scene
	 * @param asset Asset to use
	*/
	virtual void use_asset(asset::Asset* asset);

	/**
	 * @brief Removes the asset from the Scene
	 * @param asset Asset to remove
	*/
	virtual void remove_asset(asset::Asset* asset);

	// Inherited via asset_base
	virtual void prepare_load() override;
	virtual bool ready_to_load() override;
	virtual void load() override;
	virtual void unload() override;
	virtual bool is_preparing() override;
	virtual bool is_loaded() override;

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
