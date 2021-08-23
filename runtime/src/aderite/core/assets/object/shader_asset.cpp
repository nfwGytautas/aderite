#include "shader_asset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/core/assets/asset_manager.hpp"
#include "aderite/core/threading/threader.hpp"
#include "aderite/core/rendering/draw_call.hpp"

namespace aderite {
	namespace asset {
		bgfx::ShaderHandle load_shader(const std::vector<unsigned char>& source, const std::string& name) {
			const bgfx::Memory* mem = bgfx::copy(source.data(), source.size() + 1);
			mem->data[mem->size - 1] = '\0';
			bgfx::ShaderHandle vsh = bgfx::createShader(mem);
			bgfx::setName(vsh, name.c_str());
			return vsh;
		}

		shader_asset::~shader_asset() {
			if (bgfx::isValid(m_handle)) {
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

		void shader_asset::fill_draw_call(rendering::draw_call* dc) {
			dc->Shader = m_handle;
		}

		void shader_asset::load() {
			ASSERT_RENDER_THREAD;
			if (is_loaded()) {
				LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
				unload();
			}

			// Load bgfx bin shader
			bgfx::ShaderHandle vsh = load_shader(m_vertexSource, "vVertex");
			bgfx::ShaderHandle fsh = load_shader(m_fragmentSource, "fVertex");

			// Create program
			m_handle = bgfx::createProgram(vsh, fsh, true);

			m_being_prepared = false;
		}

		void shader_asset::unload() {
			ASSERT_RENDER_THREAD;
			if (bgfx::isValid(m_handle)) {
				bgfx::destroy(m_handle);
				m_handle = BGFX_INVALID_HANDLE;
			}
		}

		bool shader_asset::is_preparing() {
			return m_being_prepared;
		}

		bool shader_asset::is_loaded() {
			return bgfx::isValid(m_handle);
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
			m_vertexSource = engine::get_asset_manager()->load_bin_file(m_info.VertexPath);
			m_fragmentSource = engine::get_asset_manager()->load_bin_file(m_info.FragmentPath);
			m_being_prepared = true;
		}

		bool shader_asset::ready_to_load() {
			return (m_vertexSource.size() > 0 && m_fragmentSource.size() > 0);
		}

	}
}
