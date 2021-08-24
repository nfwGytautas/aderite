#include "project.hpp"

#include <fstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/Scene.hpp"

// Previous versions:
//	- 2021_07_31r1

constexpr const char* current_version = "2021_07_31r1";

namespace aderite {
	namespace editor {

		project::project(const std::string& dir, const std::string& name)
			: m_directory(dir), m_name(name)
		{
			m_directory.append(m_name);
		}

		project::~project() {
		}

		bool project::save() {
			std::filesystem::path root = std::filesystem::path(m_directory);

			// Check if directory exists if not then create one
			if (!std::filesystem::is_directory(root) || !std::filesystem::exists(root)) {
				std::filesystem::create_directory(root);
			}

			YAML::Emitter out;
			out << YAML::BeginMap; // Root

			// Common
			out << YAML::Key << "Version" << YAML::Value << current_version;
			out << YAML::Key << "Name" << YAML::Value << m_name;
			out << YAML::Key << "Type" << YAML::Value << "Project";

			if (engine::get_scene_manager()->current_scene()) {
				out << YAML::Key << "ActiveScene" << YAML::Value << engine::get_scene_manager()->current_scene()->getName();
				m_active_scene = engine::get_scene_manager()->current_scene()->getName();
			}

			out << YAML::EndMap; // Root

			std::ofstream fout(root.append(m_name + ".aproj"));
			fout << out.c_str();

			return true;
		}

		project* project::load(const std::string& path) {
			YAML::Node data = YAML::LoadFile(path);

			// Check version
			if (!data["Version"]) {
				LOG_ERROR("Loading scene from {0} failed because there is no version information", path);
				return nullptr;
			}

			// Check type
			if (!data["Type"]) {
				LOG_ERROR("Loading scene from {0} failed because no type information was given", path);
				return nullptr;
			}

			if (data["Type"].as<std::string>() != "Project") {
				LOG_ERROR("Trying to load asset of type {0} as a project. File {1}", data["Type"].as<std::string>(), path);
				return nullptr;
			}

			std::filesystem::path fs_path = std::filesystem::path(path);
			project* p = new project(fs_path.parent_path().parent_path().string(), fs_path.stem().string());

			if (data["ActiveScene"]) {
				p->m_active_scene = data["ActiveScene"].as<std::string>();
			}

			return p;
		}

		std::string project::get_name() const {
			return m_name;
		}

		std::filesystem::path project::get_root_dir() const {
			return m_directory;
		}

		std::string project::get_active_scene() const {
			return m_active_scene;
		}

		void project::validate() {
			if (engine::get_scene_manager()->current_scene() == nullptr && !m_active_scene.empty()) {
				save();
			}
			else if (engine::get_scene_manager()->current_scene()->getName() != m_active_scene) {
				save();
			}
		}
	}
}
