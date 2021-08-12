#pragma once

#include <vector>
#include <entt/entity/registry.hpp>
#include "aderite/core/assets/asset.hpp"
#include "aderite/core/scene/components.hpp"

namespace aderite {
	namespace asset {
		class asset_manager;
	}

	namespace scene {
		class entity;
		class scene_manager;

		/**
		 * @brief Scene object, at any given time a single scene can be active in aderite.
		 * Scenes are guaranteed to be serializable. This includes all the possible resources that it uses
		 * be it meshes, materials, etc. However these resources are loaded as trunks into the asset manager
		 * the actual data is not loaded until needed.
		*/
		class scene : public asset::asset_base {
		public:
			/**
			 * @brief Returns entt registry
			*/
			entt::registry& get_entity_registry() {
				return m_registry;
			}

			/**
			 * @brief Create entity with a meta component
			 * @return Entity instance
			*/
			entity create_entity(const components::meta& meta);

			/**
			 * @brief Destroy an entity
			*/
			void destroy_entity(entity entity);

			/**
			 * @brief Marks the asset as being used by the scene
			 * @param asset Asset to use
			*/
			virtual void use_asset(asset::asset_base* asset);

			/**
			 * @brief Removes the asset from the scene
			 * @param asset Asset to remove
			*/
			virtual void remove_asset(asset::asset_base* asset);

			// Inherited via asset_base
			virtual void prepare_load() override;
			virtual bool ready_to_load() override;
			virtual void load() override;
			virtual void unload() override;
			virtual bool is_preparing() override;
			virtual bool is_loaded() override;

			virtual asset::asset_type type() const override;
			virtual bool in_group(asset::asset_group group) const override;
		protected:
			virtual bool serialize(YAML::Emitter& out) override;
			virtual bool deserialize(YAML::Node& data) override;

			scene(const std::string& name)
				: asset_base(name)
			{}

			friend class scene_manager;
			friend class ::aderite::asset::asset_manager;
		private:
			// Assets that the scene uses
			std::vector<asset::asset_base*> m_assets;
			entt::registry m_registry;
		};

	}
}
