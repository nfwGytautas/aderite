#include "mesh_asset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/core/assets/asset_manager.hpp"
#include "aderite/core/assets/sources/mesh_source.hpp"
#include "aderite/core/threading/threader.hpp"
#include "aderite/core/rendering/draw_call.hpp"

namespace aderite {
	namespace asset {
		mesh_asset::~mesh_asset() {
			// TODO: Check for either handle to be valid
			if (bgfx::isValid(m_vbh)) {
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
			out << YAML::Key << "HasPosition" << YAML::Value << m_info.HasPosition;
			out << YAML::Key << "IsStatic" << YAML::Value << m_info.IsStatic;
			out << YAML::Key << "HasIndices" << YAML::Value << m_info.HasIndices;

			return true;
		}

		bool mesh_asset::deserialize(YAML::Node& data) {
			// TODO: Error check
			
			if (data["Source"]) {
				m_info.SourceFile = data["Source"].as<std::string>();
			}

			m_info.HasPosition = data["HasPosition"].as<bool>();
			m_info.HasIndices = data["HasIndices"].as<bool>();
			m_info.IsStatic = data["IsStatic"].as<bool>();

			return true;
		}

		void mesh_asset::fill_draw_call(rendering::draw_call* dc) {
			dc->VBO = m_vbh;
			dc->IBO = m_ibh;
		}

		void mesh_asset::load() {
			ASSERT_RENDER_THREAD;
			if (is_loaded()) {
				LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
				unload();
			}

			// Create layout
			bgfx::VertexLayout layout;
			layout.begin();

			if (m_info.HasPosition) {
				layout.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
			}

			layout.end();

			// Create handles
			if (m_info.IsStatic) {
				struct PosColorVertex
				{
					float x;
					float y;
					float z;
				};

				static PosColorVertex cubeVertices[] =
				{
					{-1.0f,  1.0f,  1.0f },
					{ 1.0f,  1.0f,  1.0f },
					{-1.0f, -1.0f,  1.0f },
					{ 1.0f, -1.0f,  1.0f },
					{-1.0f,  1.0f, -1.0f },
					{ 1.0f,  1.0f, -1.0f },
					{-1.0f, -1.0f, -1.0f },
					{ 1.0f, -1.0f, -1.0f },
				};

				static const uint16_t cubeTriList[] =
				{
					0, 1, 2,
					1, 3, 2,
					4, 6, 5,
					5, 6, 7,
					0, 2, 4,
					4, 2, 6,
					1, 5, 3,
					5, 7, 3,
					0, 4, 1,
					4, 5, 1,
					2, 3, 6,
					6, 3, 7,
				};

				auto& positionData = m_source->position_data();
				auto& indicesData = m_source->indices_data();
				m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(positionData.data(), sizeof(float) * positionData.size()), layout);
				m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(indicesData.data(), sizeof(unsigned int) * indicesData.size()));
				//m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), layout);
				//m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));
			}
			else {
				LOG_ERROR("Unimplemented dynamic mesh");
				m_being_prepared = false;
				return;
			}

			m_being_prepared = false;
		}

		void mesh_asset::unload() {
			ASSERT_RENDER_THREAD;
			if (bgfx::isValid(m_vbh)) {
				bgfx::destroy(m_vbh);
				m_vbh = BGFX_INVALID_HANDLE;
			}

			if (bgfx::isValid(m_ibh)) {
				bgfx::destroy(m_ibh);
				m_ibh = BGFX_INVALID_HANDLE;
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
			return bgfx::isValid(m_vbh);
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
				if (m_info.HasPosition) {
					source->request_position_data();
				}
				
				if (m_info.HasIndices) {
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
