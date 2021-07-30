#include "scene_manager.hpp"

#include "aderite/aderite.hpp"
#include "aderite/utility/random.hpp"

namespace aderite {
	namespace scene {
		
		bool scene_manager::init() {
			return true;
		}

		void scene_manager::shutdown() {
			for (scene* scene : m_scenes) {
				delete scene;
			}
		}

		scene* scene_manager::new_scene() {
			scene* s = new scene(get_handle());
			m_scenes.push_back(s);
			return s;
		}

		scene* scene_manager::load_file(const std::string& path) {
			scene* s = new scene("");
			if (!s->deserialize(::aderite::engine::get_asset_manager()->get_res_dir() + path)) {
				LOG_ERROR("Failed to load scene from {0}", path);
				delete s;
				return nullptr;
			}
			m_scenes.push_back(s);
			return s;
		}

		void scene_manager::set_active(scene* scene) {
			m_activeScene = scene;
		}

		scene* scene_manager::current_scene() const {
			return m_activeScene;
		}

		scene_handle scene_manager::get_handle() {
			return random::generate_uuid();
		}

	}
}