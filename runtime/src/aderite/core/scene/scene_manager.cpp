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
			for (scene* scene : m_scenes) {
				delete scene;
			}
		}

		scene* scene_manager::new_scene(const std::string& name) {
			// Check for conflicts (editor only thing so this will never be called from scripts / runtime so no need to check for binary format)
			if (std::filesystem::exists(::aderite::engine::get_asset_manager()->get_res_dir() / (name + ".scene"))) {
				return nullptr;
			}
			
			scene* s = new scene(name);
			m_scenes.push_back(s);
			return s;
		}

		scene* scene_manager::read_scene(const std::string& name) {
			scene* s = new scene("");
			if (!s->deserialize((::aderite::engine::get_asset_manager()->get_res_dir() / (name + ".scene")).string())) {
				LOG_ERROR("Failed to load scene from {0}", name);
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

		void scene_manager::save_scene(scene* scene) {
			if (!scene->serialize((::aderite::engine::get_asset_manager()->get_res_dir() / (scene->get_name() + ".scene")).string())) {
				LOG_ERROR("Failed to save scene {0}", scene->get_name());
			}
		}

	}
}