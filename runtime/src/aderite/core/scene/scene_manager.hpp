#pragma once

#include <vector>
#include "aderite/core/scene/scene.hpp"

namespace aderite {
	namespace scene {

		/**
		 * @brief Scene manager for aderite
		*/
		class scene_manager {
		public:
			/**
			 * @brief Initializes the scene manager
			*/
			bool init();

			/**
			 * @brief Frees all scene manager information
			*/
			void shutdown();

			/**
			 * @brief Creates a new scene
			 * @param name Name of the scene
			 * @return The created scene object
			*/
			scene* new_scene(const std::string& name);

			/**
			 * @brief Read the scene metadata from memory, doesn't load assets
			 * @param scene Scene name
			 * @return Loaded scene or nullptr
			*/
			scene* read_scene(const std::string& name);

			/**
			 * @brief Sets the specified scene as active
			 * @param scene New active scene
			*/
			void set_active(scene* scene);

			/**
			 * @brief Returns the current active scene or nullptr if no active scene
			*/
			scene* current_scene() const;

			/**
			 * @brief Serializes the specified scene to memory
			 * @param scene Scene to serialize
			*/
			void save_scene(scene* scene);

			auto begin() {
				return m_scenes.begin();
			}

			auto begin() const {
				return m_scenes.begin();
			}

			auto end() {
				return m_scenes.end();
			}

			auto end() const {
				return m_scenes.end();
			}
		private:
			scene_manager() {}
			friend class engine;

		private:
			scene* m_activeScene = nullptr;
			std::vector<scene*> m_scenes;
		};

	}
}
