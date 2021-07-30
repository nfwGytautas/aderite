#pragma once

#include <vector>
#include "aderite/interfaces/iserializable.hpp"
#include "aderite/utility/pointer.hpp"
#include "aderite/core/assets/asset.hpp"

namespace aderite {
	namespace scene {

		using scene_handle = std::string;

		/**
		 * @brief Scene object, at any given time a single scene can be active in aderite.
		 * Scenes are guaranteed to be serializable. This includes all the possible resources that it uses
		 * be it meshes, materials, etc. However these resources are loaded as trunks into the asset manager
		 * the actual data is not loaded until needed.
		*/
		class scene : public interfaces::iserializable {
		public:
			/**
			 * @brief Get the internal handle of the scene
			*/
			scene_handle get_handle() const {
				return m_handle;
			}

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
			 * @brief Marks the asset as being used by the scene
			 * @param asset Asset to use
			*/
			virtual void use_asset(relay_ptr<asset::asset_base> asset);

			/**
			 * @brief Removes the asset from the scene
			 * @param asset Asset to remove
			*/
			virtual void remove_asset(relay_ptr<asset::asset_base> asset);

			/**
			 * @brief Serializes the scene to file
			 * @param path Path to the scene file
			 * @return True if deserialized without errors, false otherwise
			*/
			virtual bool serialize(const std::string& path) override;

			/**
			 * @brief Deserializes the scene from the specified path
			 * @param path Path to the scene file
			 * @return True if deserialized without errors, false otherwise
			*/
			virtual bool deserialize(const std::string& path) override;
		private:
			scene(scene_handle handle)
				: m_handle(handle)
			{}

			friend class scene_manager;
		private:
			// Assets (handles) that the scene uses
			std::vector<asset::asset_handle> m_assets;

			std::string m_name = "";
			scene_handle m_handle = "";
		};

	}
}
