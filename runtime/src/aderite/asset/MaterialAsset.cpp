#include "MaterialAsset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Utility.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/TextureAsset.hpp"
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

	out << YAML::Key << "Samplers" << YAML::BeginSeq;
	for (auto pair : m_info.Samplers) {
		out << YAML::BeginMap; 
		out << YAML::Key << "Name" << YAML::Value << pair.first;
		out << YAML::Key << "Texture" << YAML::Value;

		if (pair.second != nullptr) {
			out << pair.second->getName();
		}
		else {
			out << YAML::Null;
		}

		out << YAML::EndMap;
	}
	out << YAML::EndSeq;


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
			LOG_WARN("Size and type mismatch for {0}, trying to reapply type", p_name);
			// Reapply type
			setType(m_info.Type);
			return true;
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

	for (YAML::Node& sampler : data["Samplers"]) {
		TextureAsset* texture = nullptr;

		if (!sampler["Texture"].IsNull()) {
			texture = static_cast<TextureAsset*>(::aderite::Engine::getAssetManager()->getOrRead(sampler["Texture"].as<std::string>()));
		}

		m_info.Samplers[sampler["Name"].as<std::string>()] = texture;
	}

	return true;
}

void MaterialAsset::load() {
	if (isLoaded()) {
		LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
		unload();
	}

	m_info.Type->load();
	for (auto samplers : m_info.Samplers) {
		if (samplers.second) {
			samplers.second->load();
		}
	}
	m_isBeingPrepared = false;
}

void MaterialAsset::unload() {
	//m_info.Shader->unload();
	// TODO: Rework cause this is reference counted
	for (auto samplers : m_info.Samplers) {
		if (samplers.second) {
			samplers.second->unload();
		}
	}
	m_info.Type->unload();
}

bool MaterialAsset::isPreparing() {
	return m_isBeingPrepared;
}

bool MaterialAsset::isLoaded() {
	for (auto samplers : m_info.Samplers) {
		if (samplers.second) {
			if (!samplers.second->isLoaded()) {
				return false;
			}
		}
	}
	return m_info.Type->isLoaded();
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
	// TODO: Move to editor
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

	// Samplers
	m_info.Samplers.clear();
	for (prop::Property* prop : type->getFields().Properties) {
		if (prop::isSampler(prop->getType())) {
			m_info.Samplers[prop->getName()] = nullptr;
		}
	}
}

std::vector<std::pair<bgfx::UniformHandle, bgfx::TextureHandle>> MaterialAsset::getSamplerData() const {
	std::vector<std::pair<bgfx::UniformHandle, bgfx::TextureHandle>> result;

	auto typeSamplers = m_info.Type->getSamplers();
	for (auto sampler : m_info.Samplers) {
		if (!sampler.second) {
			result.push_back(std::make_pair(
				typeSamplers[sampler.first], bgfx::TextureHandle{bgfx::kInvalidHandle}
			));

			continue;
		}

		result.push_back(std::make_pair(
			typeSamplers[sampler.first], sampler.second->getHandle()
		));
	}

	return result;
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
	m_info.Type->prepareLoad();
	for (auto samplers : m_info.Samplers) {
		if (samplers.second) {
			samplers.second->prepareLoad();
		}
	}
	m_isBeingPrepared = true;
}

bool MaterialAsset::isReadyToLoad() {
	for (auto samplers : m_info.Samplers) {
		if (samplers.second) {
			if (!samplers.second->isReadyToLoad()) {
				return false;
			}
		}
	}
	return m_info.Type->isReadyToLoad();
	//return true;
}

ADERITE_ASSET_NAMESPACE_END
