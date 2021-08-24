#include "MaterialAsset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/ShaderAsset.hpp"
#include "aderite/rendering/DrawCall.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

MaterialAsset::~MaterialAsset() {

}

AssetType MaterialAsset::type() const {
	return AssetType::MATERIAL;
}

bool MaterialAsset::serialize(YAML::Emitter& out) {
	// Material
	if (m_info.Shader != nullptr) {
		out << YAML::Key << "Shader" << YAML::Value << m_info.Shader->getName();
	}

	return true;
}

bool MaterialAsset::deserialize(YAML::Node& data) {
	if (data["Shader"]) {
		std::string shaderName = data["Shader"].as<std::string>();

		// TODO: Error check
		m_info.Shader = engine::get_AssetManager()->get_or_read(shaderName);
	}

	return true;
}

void MaterialAsset::fill_draw_call(rendering::DrawCall* dc) {
	static_cast<asset::shader_asset*>(m_info.Shader)->fill_draw_call(dc);
}

void MaterialAsset::load() {
	if (is_loaded()) {
		LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
		unload();
	}

	m_info.Shader->load();
	m_being_prepared = false;
}

void MaterialAsset::unload() {
	m_info.Shader->unload();
}

bool MaterialAsset::is_preparing() {
	return m_being_prepared;
}

bool MaterialAsset::is_loaded() {
	return m_info.Shader->is_loaded();
}

MaterialAsset::MaterialAsset(const std::string& name)
	: Asset(name)
{}

MaterialAsset::MaterialAsset(const std::string& name, const fields& info)
	: Asset(name), m_info(info)
{}

bool MaterialAsset::isInGroup(AssetGroup group) const {
	switch (group) {
	default:
		return false;
	}
}

void MaterialAsset::prepare_load() {
	// Load sources
	m_info.Shader->prepare_load();
	m_being_prepared = true;
}

bool MaterialAsset::ready_to_load() {
	return m_info.Shader->ready_to_load();
}

ADERITE_ASSET_NAMESPACE_END
