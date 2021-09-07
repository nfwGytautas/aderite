#include "MaterialAsset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Utility.hpp"
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
	//// Material
	//if (m_info.Shader != nullptr) {
	//	out << YAML::Key << "Shader" << YAML::Value << m_info.Shader->getName();
	//}

	return true;
}

bool MaterialAsset::deserialize(YAML::Node& data) {
	if (data["Shader"]) {
		std::string shaderName = data["Shader"].as<std::string>();

		// TODO: Error check
		//m_info.Shader = ::aderite::Engine::getAssetManager()->getOrRead(shaderName);
	}

	return true;
}

void MaterialAsset::fillDrawCall(rendering::DrawCall* dc) {
	//static_cast<asset::ShaderAsset*>(m_info.Shader)->fillDrawCall(dc);
}

void MaterialAsset::load() {
	if (isLoaded()) {
		LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
		unload();
	}

	//m_info.Shader->load();
	m_isBeingPrepared = false;
}

void MaterialAsset::unload() {
	//m_info.Shader->unload();
}

bool MaterialAsset::isPreparing() {
	return m_isBeingPrepared;
}

bool MaterialAsset::isLoaded() {
	//return m_info.Shader->isLoaded();
	return true;
}

size_t MaterialAsset::hash() const {
	size_t thisHash = std::hash<std::string>{}(p_name);
	//if (m_info.Shader) {
	//	size_t shaderHash = m_info.Shader->hash();
	//	return utility::combineHash(shaderHash, thisHash);
	//}

	return thisHash;
}

MaterialAsset::MaterialAsset(const std::string& name)
	: Asset(name + ".material")
{}

MaterialAsset::MaterialAsset(const std::string& name, const fields& info)
	: Asset(name + ".material"), m_info(info)
{}

bool MaterialAsset::isInGroup(AssetGroup group) const {
	switch (group) {
	default:
		return false;
	}
}

void MaterialAsset::prepareLoad() {
	// Load sources
	//m_info.Shader->prepareLoad();
	m_isBeingPrepared = true;
}

bool MaterialAsset::isReadyToLoad() {
	//return m_info.Shader->isReadyToLoad();
	return true;
}

ADERITE_ASSET_NAMESPACE_END
