#include "MeshAsset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MeshSource.hpp"
#include "aderite/rendering/DrawCall.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

MeshAsset::~MeshAsset() {
	// TODO: Check for either handle to be valid
	if (bgfx::isValid(m_vbh)) {
		LOG_WARN("Deleting a loaded mesh asset {0}", getName());
	}
}

AssetType MeshAsset::type() const {
	return AssetType::MESH;
}

bool MeshAsset::serialize(YAML::Emitter& out) {
	// Mesh
	out << YAML::Key << "Source" << YAML::Value << m_info.SourceFile;
	
	// Layout
	out << YAML::Key << "HasPosition" << YAML::Value << m_info.HasPosition;
	out << YAML::Key << "IsStatic" << YAML::Value << m_info.IsStatic;
	out << YAML::Key << "HasIndices" << YAML::Value << m_info.HasIndices;

	return true;
}

bool MeshAsset::deserialize(YAML::Node& data) {
	// TODO: Error check
	
	if (data["Source"]) {
		m_info.SourceFile = data["Source"].as<std::string>();
	}

	m_info.HasPosition = data["HasPosition"].as<bool>();
	m_info.HasIndices = data["HasIndices"].as<bool>();
	m_info.IsStatic = data["IsStatic"].as<bool>();

	return true;
}

void MeshAsset::fill_draw_call(rendering::DrawCall* dc) {
	dc->VBO = m_vbh;
	dc->IBO = m_ibh;
}

void MeshAsset::load() {
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
		auto& positionData = m_source->position_data();
		auto& indicesData = m_source->indices_data();
		m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(positionData.data(), sizeof(float) * positionData.size()), layout);
		m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(indicesData.data(), sizeof(unsigned int) * indicesData.size()), BGFX_BUFFER_INDEX32);
	}
	else {
		LOG_ERROR("Unimplemented dynamic mesh");
		m_being_prepared = false;
		return;
	}

	m_being_prepared = false;
}

void MeshAsset::unload() {
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

bool MeshAsset::is_preparing() {
	return m_being_prepared;
}

bool MeshAsset::is_loaded() {
	return bgfx::isValid(m_vbh);
}

MeshAsset::MeshAsset(const std::string& name)
	: Asset(name)
{}

MeshAsset::MeshAsset(const std::string& name, const fields& info)
	: Asset(name), m_info(info)
{}

bool MeshAsset::isInGroup(AssetGroup group) const {
	switch (group) {
	case AssetGroup::DEPENDS_ON_RAW:
	{
		return true;
	}
	default:
		return false;
	}
}

void MeshAsset::prepare_load() {
	// Load sources
	engine::get_AssetManager()->load_MeshSource(m_info.SourceFile, [&](asset::MeshSource* source) {
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

bool MeshAsset::ready_to_load() {
	return m_source != nullptr && (m_source->error() == asset::MeshSource::load_error::NONE);
}

ADERITE_ASSET_NAMESPACE_END
