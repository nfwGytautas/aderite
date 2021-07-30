#include "shader_asset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/utility/macros.hpp"

// Previous versions:
//	- 2021_07_13r1

constexpr const char* current_version = "2021_07_13r1";

namespace aderite {
	namespace asset {
		shader_asset::~shader_asset() {
			unload();
		}

		asset_type shader_asset::type() const {
			return asset_type::SHADER;
		}

		bool shader_asset::serialize(const std::string& path) {
			YAML::Emitter out;
			out << YAML::BeginMap;
			
			// Common
			out << YAML::Key << "Version" << YAML::Value << current_version;
			out << YAML::Key << "Handle" << YAML::Value << p_handle;
			out << YAML::Key << "Name" << YAML::Value << p_name;
			out << YAML::Key << "Type" << YAML::Value << "Shader";

			// Shader
			out << YAML::Key << "Vertex" << YAML::Value << m_info.VertexPath;
			out << YAML::Key << "Fragment" << YAML::Value << m_info.FragmentPath;

			out << YAML::EndMap;

			std::ofstream fout(path);
			fout << out.c_str();

			return true;
		}

		bool shader_asset::deserialize(const std::string& path) {
			YAML::Node data = YAML::LoadFile(path);

			// Check version
			if (!data["Version"]) {
				LOG_ERROR("Loading shader from {0} failed because there is no version information", path);
				return false;
			}

			// Check type
			if (!data["Type"]) {
				LOG_ERROR("Loading shader from {0} failed because no type information was given", path);
				return false;
			}

			if (data["Type"].as<std::string>() != "Shader") {
				LOG_ERROR("Trying to load asset of type {0} as a shader. File {1}", data["Type"].as<std::string>(), path);
				return false;
			}

			p_handle = data["Handle"].as<std::string>();
			p_name = data["Name"].as<std::string>();
			m_info.VertexPath = data["Vertex"].as<std::string>();
			m_info.FragmentPath = data["Fragment"].as<std::string>();

			return true;
		}

		void shader_asset::load() {
			ASSERT_RENDER_THREAD;
			if (p_loaded) {
				LOG_WARN("Loading an already loaded asset {0} {1}, is this intended?", p_handle, p_name);
				unload();
			}

			m_shader = shader::create({
				m_vertexSource,
				m_fragmentSource
			});

			p_loaded = true;
		}

		void shader_asset::unload() {
			ASSERT_RENDER_THREAD;
			delete m_shader;
			m_shader = nullptr;
			p_loaded = false;
		}

		shader_asset::shader_asset(asset_handle handle, const fields& info)
			: asset_base(handle), m_info(info)
		{}

		bool shader_asset::in_group(asset_group group) const {
			switch (group) {
			case asset_group::SHADER:
				return true;
			default:
				return false;
			}
		}

		void shader_asset::prepare_load() {
			// Load sources
			m_vertexSource = engine::get_asset_manager()->load_txt_file(m_info.VertexPath);
			m_fragmentSource = engine::get_asset_manager()->load_txt_file(m_info.FragmentPath);
		}

		bool shader_asset::ready_to_load() {
			return (m_vertexSource != "" && m_fragmentSource != "");
		}

	}
}
