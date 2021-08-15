#include "shader_asset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/core/assets/asset_manager.hpp"
#include "aderite/core/threading/threader.hpp"
#include "aderite/core/rendering/shader/shader.hpp"

namespace aderite {
	namespace asset {
		shader_asset::~shader_asset() {
			if (m_shader) {
				LOG_WARN("Deleting a loaded shader asset {0}", get_name());
			}
		}

		asset_type shader_asset::type() const {
			return asset_type::SHADER;
		}

		bool shader_asset::serialize(YAML::Emitter& out) {
			// Shader
			out << YAML::Key << "Vertex" << YAML::Value << m_info.VertexPath;
			out << YAML::Key << "Fragment" << YAML::Value << m_info.FragmentPath;

			return true;
		}

		bool shader_asset::deserialize(YAML::Node& data) {
			// TODO: Error check
			m_info.VertexPath = data["Vertex"].as<std::string>();
			m_info.FragmentPath = data["Fragment"].as<std::string>();

			return true;
		}

		void shader_asset::load() {
			ASSERT_RENDER_THREAD;
			if (is_loaded()) {
				LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
				unload();
			}

			m_shader = shader::create({
				m_vertexSource,
				m_fragmentSource
			});

			m_being_prepared = false;
		}

		void shader_asset::unload() {
			ASSERT_RENDER_THREAD;
			if (m_shader) {
				delete m_shader;
				m_shader = nullptr;
			}
		}

		bool shader_asset::is_preparing() {
			return m_being_prepared;
		}

		bool shader_asset::is_loaded() {
			return m_shader != nullptr;
		}

		shader_asset::shader_asset(const std::string& name)
			: asset_base(name)
		{}

		shader_asset::shader_asset(const std::string& name, const fields& info)
			: asset_base(name), m_info(info)
		{}

		bool shader_asset::in_group(asset_group group) const {
			switch (group) {
			case asset_group::DEPENDS_ON_RAW:
			{
				return true;
			}
			default:
				return false;
			}
		}

		void shader_asset::prepare_load() {
			// Load sources
			// TODO: Async
			m_vertexSource = engine::get_asset_manager()->load_txt_file(m_info.VertexPath);
			m_fragmentSource = engine::get_asset_manager()->load_txt_file(m_info.FragmentPath);
			m_being_prepared = true;
		}

		bool shader_asset::ready_to_load() {
			return (m_vertexSource != "" && m_fragmentSource != "");
		}

	}
}
