#pragma once

#include <string>
#include "aderite/core/scene/Entity.hpp"

namespace aderite {
	namespace asset {
		class Asset;
	}

	namespace editor {

		/**
		 * @brief Event router sink, containing functions that are invoked from editor components
		*/
		class ievent_sink {
		public:
			virtual ~ievent_sink() {}

			/**
			 * @brief Invoked when the current selected Entity is changed
			*/
			virtual void selected_entity_changed(scene::Entity& Entity) = 0;

			/**
			 * @brief Creates a new project
			 * @param dir Directory where the project will be created
			 * @param name Name of the project
			*/
			virtual void new_project(const std::string& dir, const std::string& name) = 0;

			/**
			 * @brief Save the project
			*/
			virtual void save_project() = 0;

			/**
			 * @brief Load the specified project
			 * @param path Path to project file
			*/
			virtual void load_project(const std::string& path) = 0;

			/**
			 * @brief Create new Scene
			 * @param name Name of the Scene
			*/
			virtual void new_scene(const std::string& name) = 0;

			/**
			 * @brief Create new Entity for current active Scene
			 * @param name Name of the Entity
			*/
			virtual void create_entity(const std::string& name) = 0;

			/**
			 * @brief Destroy Entity from current active Scene
			 * @param Entity Entity to destroy
			*/
			virtual void destroy_entity(const scene::Entity& Entity) = 0;

			/**
			 * @brief Invoked when the selected asset has been changed
			 * @param asset New selected asset
			*/
			virtual void selected_asset_changed(asset::Asset* asset) = 0;
		};
	}
}
