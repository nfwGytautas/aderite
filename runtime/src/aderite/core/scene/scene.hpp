#pragma once

#include <vector>
#include <entt/entity/registry.hpp>
#include "aderite/interfaces/iserializable.hpp"
#include "aderite/interfaces/iloadable.hpp"
#include "aderite/utility/pointer.hpp"
#include "aderite/core/assets/asset.hpp"
#include "aderite/core/scene/components.hpp"

namespace aderite {
	namespace scene {

		class entity;

		/**
		 * @brief Scene object, at any given time a single scene can be active in aderite.
		 * Scenes are guaranteed to be serializable. This includes all the possible resources that it uses
		 * be it meshes, materials, etc. However these resources are loaded as trunks into the asset manager
		 * the actual data is not loaded until needed.
		*/
		class scene : public interfaces::iserializable, public interfaces::iloadable {
		public:
			/**
			 * @brief Set the name of the scene
			*/
			void set_name(const std::string name) {
				m_name = name;
			}

			/**
			 * @brief Get the name of the scene
			*/
			std::string get_name() const {
				return m_name;
			}

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

			// Inherited via iloadable
			virtual void prepare_load() override;
			virtual bool ready_to_load() override;
			virtual void load() override;
			virtual void unload() override;
			virtual bool is_preparing() override;
		private:
			/**
			 * @brief Serializes the scene to file
			 * @param path Path to the scene file
			 * @return True if serialized without errors, false otherwise
			*/
			virtual bool serialize(const std::string& path) override;

			/**
			 * @brief Deserializes the scene from the specified path
			 * @param path Path to the scene file
			 * @return True if deserialized without errors, false otherwise
			*/
			virtual bool deserialize(const std::string& path) override;

			scene(const std::string& name)
				: m_name(name)
			{}

			friend class scene_manager;
		private:
			// Assets that the scene uses
			std::vector<asset::asset_base*> m_assets;
			entt::registry m_registry;

			std::string m_name = "";
		};

	}
}
