#include "material_asset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/core/assets/asset_manager.hpp"
#include "aderite/core/threading/threader.hpp"

namespace aderite {
	namespace asset {
		material_asset::~material_asset() {

		}

		asset_type material_asset::type() const {
			return asset_type::MATERIAL;
		}

		bool material_asset::serialize(YAML::Emitter& out) {
			// Material
			if (m_info.Shader != nullptr) {
				out << YAML::Key << "Shader" << YAML::Value << m_info.Shader->get_name();
			}

			return true;
		}

		bool material_asset::deserialize(YAML::Node& data) {
			if (data["Shader"]) {
				std::string shaderName = data["Shader"].as<std::string>();

				// TODO: Error check
				m_info.Shader = engine::get_asset_manager()->get_or_read(shaderName);
			}

			return true;
		}

		void material_asset::load() {
			ASSERT_RENDER_THREAD;
			if (is_loaded()) {
				LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
				unload();
			}

			m_info.Shader->load();
			m_being_prepared = false;
		}

		void material_asset::unload() {
			ASSERT_RENDER_THREAD;
			m_info.Shader->unload();
		}

		bool material_asset::is_preparing() {
			return m_being_prepared;
		}

		bool material_asset::is_loaded() {
			return m_info.Shader->is_loaded();
		}

		material_asset::material_asset(const std::string& name)
			: asset_base(name)
		{}

		material_asset::material_asset(const std::string& name, const fields& info)
			: asset_base(name), m_info(info)
		{}

		bool material_asset::in_group(asset_group group) const {
			switch (group) {
			default:
				return false;
			}
		}

		void material_asset::prepare_load() {
			// Load sources
			m_info.Shader->prepare_load();
			m_being_prepared = true;
		}

		bool material_asset::ready_to_load() {
			return m_info.Shader->ready_to_load();
		}

	}
}
