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
			 * @brief Sets the specified scene as active
			 * @param scene New active scene
			*/
			void set_active(scene* scene);

			/**
			 * @brief Returns the current active scene or nullptr if no active scene
			*/
			scene* current_scene() const;
		private:
			scene_manager() {}
			friend class engine;

		private:
			scene* m_activeScene = nullptr;
		};

	}
}
