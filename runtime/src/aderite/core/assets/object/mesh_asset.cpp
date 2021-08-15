#include "mesh_asset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/core/assets/asset_manager.hpp"
#include "aderite/core/assets/sources/mesh_source.hpp"
#include "aderite/core/rendering/vao/vao.hpp"
#include "aderite/core/threading/threader.hpp"

namespace aderite {
	namespace asset {
		mesh_asset::~mesh_asset() {
			if (m_vao) {
				LOG_WARN("Deleting a loaded mesh asset {0}", get_name());
			}
		}

		asset_type mesh_asset::type() const {
			return asset_type::MESH;
		}

		bool mesh_asset::serialize(YAML::Emitter& out) {
			// Mesh
			out << YAML::Key << "Source" << YAML::Value << m_info.SourceFile;
			
			// Layout
			out << YAML::Key << "Layout";
			out << YAML::BeginMap; // Layout

			out << YAML::Key << "HasPosition" << YAML::Value << m_info.Layout.HasPosition;
			out << YAML::Key << "IsPositionStatic" << YAML::Value << m_info.Layout.IsPositionStatic;
			out << YAML::Key << "PositionStart" << YAML::Value << m_info.Layout.PositionStart;
			out << YAML::Key << "HasIndices" << YAML::Value << m_info.Layout.HasIndices;

			out << YAML::EndMap; // Layout

			return true;
		}

		bool mesh_asset::deserialize(YAML::Node& data) {
			// TODO: Error check
			
			if (data["Source"]) {
				m_info.SourceFile = data["Source"].as<std::string>();
			}

			m_info.Layout.HasPosition = data["Layout"]["HasPosition"].as<bool>();
			m_info.Layout.HasIndices = data["Layout"]["HasIndices"].as<bool>();
			m_info.Layout.IsPositionStatic = data["Layout"]["IsPositionStatic"].as<bool>();
			m_info.Layout.PositionStart = data["Layout"]["PositionStart"].as<size_t>();

			return true;
		}

		void mesh_asset::load() {
			ASSERT_RENDER_THREAD;
			if (is_loaded()) {
				LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
				unload();
			}

			m_vao = vao::create();

			m_info.Layout.IndicesData = m_source->indices_data().data();
			m_info.Layout.IndicesCount = m_source->indices_data().size();
			m_info.Layout.PositionData = m_source->position_data().data();
			m_info.Layout.PositionCount = m_source->position_data().size();

			m_vao->change_layout(m_info.Layout);

			m_being_prepared = false;
		}

		void mesh_asset::unload() {
			ASSERT_RENDER_THREAD;
			if (m_vao != nullptr) {
				delete m_vao;
				m_vao = nullptr;
			}

			if (m_source != nullptr) {
				delete m_source;
				m_source = nullptr;
			}
		}

		bool mesh_asset::is_preparing() {
			return m_being_prepared;
		}

		bool mesh_asset::is_loaded() {
			return m_vao != nullptr;
		}

		mesh_asset::mesh_asset(const std::string& name)
			: asset_base(name)
		{}

		mesh_asset::mesh_asset(const std::string& name, const fields& info)
			: asset_base(name), m_info(info)
		{}

		bool mesh_asset::in_group(asset_group group) const {
			switch (group) {
			case asset_group::DEPENDS_ON_RAW:
			{
				return true;
			}
			default:
				return false;
			}
		}

		void mesh_asset::prepare_load() {
			// Load sources
			engine::get_asset_manager()->load_mesh_source(m_info.SourceFile, [&](asset::mesh_source* source) {
				if (m_info.Layout.HasPosition) {
					source->request_position_data();
				}

				if (m_info.Layout.HasIndices) {
					source->request_indices_data();
				}

				// Start loading the data
				source->load();

				if (m_source != nullptr) {
					delete m_source;
				}

				m_source = source;
			});
			m_being_prepared = true;
		}

		bool mesh_asset::ready_to_load() {
			return m_source != nullptr && (m_source->error() == asset::mesh_source::load_error::NONE);
		}

	}
}
