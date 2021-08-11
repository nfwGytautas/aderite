#include "scene_manager.hpp"

#include "aderite/aderite.hpp"
#include "aderite/utility/random.hpp"
#include "aderite/utility/macros.hpp"

namespace aderite {
	namespace scene {
		
		bool scene_manager::init() {
			return true;
		}

		void scene_manager::shutdown() {

		}

		void scene_manager::set_active(scene* scene) {
			ASSERT_RENDER_THREAD;

			if (scene->is_loaded()) {
				m_activeScene = scene;
				return;
			}

			// Check if it's preparing to be loaded if not then prepare
			if (!scene->is_preparing()) {
				scene->prepare_load();
			}

			// Wait until the scene 
			while (!scene->ready_to_load()) {
				// TODO: Loading screen

				// Sleep for 1 second
				::aderite::engine::get_threader()->sleep_caller(1000);
			}
			scene->load();

			if (scene->is_loaded()) {
				m_activeScene = scene;
				return;
			}

			m_activeScene = scene;
		}

		scene* scene_manager::current_scene() const {
			return m_activeScene;
		}
	}
}