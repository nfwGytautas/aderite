#include "MaterialAsset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Utility.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/rendering/DrawCall.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

MaterialAsset::~MaterialAsset() {
	std::free(m_udata);
}

AssetType MaterialAsset::type() const {
	return AssetType::MATERIAL;
}

bool MaterialAsset::serialize(YAML::Emitter& out) {
	// Material
	if (m_info.Type != nullptr) {
		out << YAML::Key << "MaterialType" << YAML::Value << m_info.Type->getName();
	}

	out << YAML::Key << "Properties" << YAML::BeginMap;
	out << YAML::Key << "Size" << YAML::Value << m_dataSize;
	out << YAML::Key << "Data" << YAML::Flow << YAML::BeginSeq;
	
	for (size_t i = 0; i < m_dataSize / sizeof(float); i++) {
		float val = m_udata[i];
		out << m_udata[i];
	}

	out << YAML::EndSeq;
	out << YAML::EndMap;

	return true;
}

bool MaterialAsset::deserialize(YAML::Node& data) {
	if (data["MaterialType"]) {
		std::string typeName = data["MaterialType"].as<std::string>();

		// TODO: Error check
		m_info.Type = static_cast<MaterialTypeAsset*>(::aderite::Engine::getAssetManager()->getOrRead(typeName));
	}

	m_dataSize = data["Properties"]["Size"].as<size_t>();

	if (m_info.Type != nullptr) {
		if (m_info.Type->getSizeInBytes() != m_dataSize) {
			LOG_ERROR("Size and type mismatch for {0}", p_name);
			return false;
		}
	}
	else {
		if (m_dataSize > 0) {
			LOG_ERROR("Size is more than 0 on a material with no type {0}", p_name);
			return false;
		}
	}

	m_udata = static_cast<float*>(std::malloc(m_dataSize));

	if (m_udata == nullptr) {
		LOG_ERROR("Failed to allocate space for material property data {0}", p_name);
		return false;
	}

	std::memset(m_udata, 0, m_dataSize);

	YAML::Node& d = data["Properties"]["Data"];

	if (d.size() > (m_dataSize / sizeof(float))) {
		LOG_ERROR("Incorrect size for stored material data and type {0}", p_name);
		return false;
	}

	for (size_t i = 0; i < d.size(); i++) {
		m_udata[i] = d[i].as<float>();
	}

	return true;
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

void MaterialAsset::setType(MaterialTypeAsset* type) { 
	m_info.Type = type;
	
	if (m_dataSize != type->getSizeInBytes()) {
		// Recreate buffer
		m_dataSize = type->getSizeInBytes();

		if (m_udata != nullptr) {
			std::free(m_udata);
		}

		m_udata = static_cast<float*>(std::malloc(m_dataSize));
	}

	std::memset(m_udata, 0, m_dataSize);
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
