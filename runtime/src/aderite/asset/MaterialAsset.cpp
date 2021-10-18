#include "MaterialAsset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Utility.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/io/RuntimeSerializables.hpp"
#include "aderite/io/Serializer.hpp"

namespace aderite {
namespace asset {

MaterialAsset::~MaterialAsset() {
	std::free(m_udata);
}

void MaterialAsset::load(const io::Loader* loader) {
	// TODO: Reference 
}

void MaterialAsset::unload() {
	// TODO: Rework cause this is reference counted
}

io::SerializableType MaterialAsset::getType() {
	return static_cast<io::SerializableType>(io::RuntimeSerializables::MATERIAL);
}

bool MaterialAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	// Material
	if (m_info.Type != nullptr) {
		emitter << YAML::Key << "MaterialType" << YAML::Value << m_info.Type->getHandle();
	}

	emitter << YAML::Key << "Properties" << YAML::BeginMap;
	emitter << YAML::Key << "Size" << YAML::Value << m_dataSize;

	emitter << YAML::Key << "Data" << YAML::Flow << YAML::BeginSeq;
	for (size_t i = 0; i < m_dataSize / sizeof(float); i++) {
		float val = m_udata[i];
		emitter << m_udata[i];
	}
	emitter << YAML::EndSeq;
	emitter << YAML::EndMap;

	emitter << YAML::Key << "Samplers" << YAML::BeginSeq;
	for (auto pair : m_info.Samplers) {
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Name" << YAML::Value << pair.first;
		emitter << YAML::Key << "Texture" << YAML::Value;

		if (pair.second != nullptr) {
			emitter << pair.second->getHandle();
		}
		else {
			emitter << YAML::Null;
		}

		emitter << YAML::EndMap;
	}
	emitter << YAML::EndSeq;

	return true;
}

bool MaterialAsset::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	if (data["MaterialType"]) {
	    io::SerializableHandle typeName = data["MaterialType"].as<io::SerializableHandle>();

		// TODO: Error check
		m_info.Type = static_cast<MaterialTypeAsset*>(::aderite::Engine::getSerializer()->getOrRead(typeName));
	}

	m_dataSize = data["Properties"]["Size"].as<size_t>();

	if (m_info.Type != nullptr) {
		if (m_info.Type->getSizeInBytes() != m_dataSize) {
			LOG_WARN("Size and type mismatch for {0}, trying to reapply type", getHandle());
			// Reapply type
			setType(m_info.Type);
			return true;
		}
	}
	else {
		if (m_dataSize > 0) {
			LOG_ERROR("Size is more than 0 on a material with no type {0}", getHandle());
			return false;
		}
	}

	m_udata = static_cast<float*>(std::malloc(m_dataSize));

	if (m_udata == nullptr) {
		LOG_ERROR("Failed to allocate space for material property data {0}", getHandle());
		return false;
	}

	std::memset(m_udata, 0, m_dataSize);

	const YAML::Node& d = data["Properties"]["Data"];

	if (d.size() > (m_dataSize / sizeof(float))) {
		LOG_ERROR("Incorrect size for stored material data and type {0}", getHandle());
		return false;
	}

	for (size_t i = 0; i < d.size(); i++) {
		m_udata[i] = d[i].as<float>();
	}

	for (const YAML::Node& sampler : data["Samplers"]) {
		TextureAsset* texture = nullptr;

		if (!sampler["Texture"].IsNull()) {
			texture = static_cast<TextureAsset*>(::aderite::Engine::getSerializer()->getOrRead(sampler["Texture"].as<io::SerializableHandle>()));
		}

		m_info.Samplers[sampler["Name"].as<std::string>()] = texture;
	}

	return true;
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
				typeSamplers[sampler.first], bgfx::TextureHandle{ bgfx::kInvalidHandle }
			));

			continue;
		}

		result.push_back(std::make_pair(
			typeSamplers[sampler.first], sampler.second->getTextureHandle()
		));
	}

	return result;
}

}
}
