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
			m_activeScene = scene;
		}

		scene* scene_manager::current_scene() const {
			return m_activeScene;
		}
	}
}