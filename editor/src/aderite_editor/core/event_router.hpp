#pragma once

#include <functional>
#include <string>
#include "aderite/core/scene/entity.hpp"

namespace aderite {
	namespace editor {

		/**
		 * @brief Event router sink, containing functions that are invoked from editor components
		*/
		class ievent_sink {
		public:
			virtual ~ievent_sink() {}

			/**
			 * @brief Invoked when the current selected entity is changed
			*/
			virtual void selected_entity_changed(scene::entity& entity) = 0;

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
			 * @brief Create new scene
			 * @param name Name of the scene
			*/
			virtual void new_scene(const std::string& name) = 0;

			/**
			 * @brief Create new entity for current active scene
			 * @param name Name of the entity
			*/
			virtual void create_entity(const std::string& name) = 0;

			/**
			 * @brief Destroy entity from current active scene
			 * @param entity Entity to destroy
			*/
			virtual void destroy_entity(const scene::entity& entity) = 0;
		};
	}
}
