#include "MaterialTypeAsset.hpp"

#include "aderite/utility/Log.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

MaterialTypeAsset::~MaterialTypeAsset() {
	for (prop::Property* pb : m_info.Properties) {
		delete pb;
	}
}

AssetType MaterialTypeAsset::type() const {
	return AssetType::MATERIAL_TYPE;
}

bool MaterialTypeAsset::isInGroup(AssetGroup group) const {
	switch (group) {
	default:
		return false;
	}
}

void MaterialTypeAsset::prepareLoad() {

}

bool MaterialTypeAsset::isReadyToLoad() {
	return true;
}

void MaterialTypeAsset::load() {
	// TODO: Create shader, uniform, samplers
}

void MaterialTypeAsset::unload() {
	// TODO: Delete shader, uniform, samplers
}

bool MaterialTypeAsset::isPreparing() {
	return false;
}

bool MaterialTypeAsset::isLoaded() {
	return true;
}

size_t MaterialTypeAsset::hash() const {
	size_t thisHash = std::hash<std::string>{}(p_name);
	return thisHash;
}

void MaterialTypeAsset::recalculate() {
	// This method will recalculate offsets so that all properties sit in this configuration
	// although there are multiple loops over properties, most material types won't have that many properties
	// and the method should never be called on runtime
	// vec4
	// vec3 float
	// vec2 vec2
	// vec2 float float
	// float float float float

	uint16_t offset = 0;
	m_info.ElementCount = 0;

	// Temporary vector for storing floats
	std::vector<prop::Property*> floatProperties;
	// Temporary for storing vec2 carry over
	prop::Property* vec2Carry = nullptr;

	// Collect float 1 and set vec4 offsets
	for (prop::Property* prop : m_info.Properties) {
		if (prop->getType() == prop::PropertyType::FLOAT) {
			floatProperties.push_back(prop);
		}
		else if (prop->getType() == prop::PropertyType::VEC4) {
			// Set offset
			prop->setOffset(offset);
			m_info.ElementCount += prop::getElementCountForType(prop->getType());
			offset = m_info.ElementCount;
		}

		// Ignore others
	}

	// Now vec4 are ordered, go to vec3
	for (prop::Property* prop : m_info.Properties) {
		if (prop->getType() == prop::PropertyType::VEC3) {
			prop->setOffset(offset);
			m_info.ElementCount += prop::getElementCountForType(prop->getType());
			offset = m_info.ElementCount;

			// Check for float to pad the back
			if (floatProperties.size() > 0) {
				prop::Property* fProp = floatProperties.back();
				floatProperties.pop_back();
				fProp->setOffset(offset);
			}

			// No matter what add pad float
			m_info.ElementCount += 1;
			offset = m_info.ElementCount;
		}
	}

	// Now vec2
	for (prop::Property* prop : m_info.Properties) {
		if (prop->getType() == prop::PropertyType::VEC2) {
			prop->setOffset(offset);
			m_info.ElementCount += prop::getElementCountForType(prop->getType());
			offset = m_info.ElementCount;
		}
	}

	// Left over floats
	for (prop::Property* prop : floatProperties) {
		prop->setOffset(offset);
		m_info.ElementCount += prop::getElementCountForType(prop->getType());
		offset = m_info.ElementCount;
	}
}

size_t MaterialTypeAsset::getSizeInBytes() const {
	return m_info.ElementCount * sizeof(float);
}

MaterialTypeAsset::MaterialTypeAsset(const std::string& name)
	: Asset(name + ".mtype")
{}

MaterialTypeAsset::MaterialTypeAsset(const std::string& name, const fields& info)
	: Asset(name + ".mtype"), m_info(info)
{}

bool MaterialTypeAsset::serialize(YAML::Emitter& out) {
	out << YAML::Key << "ElementCount" << YAML::Value << m_info.ElementCount;

	out << YAML::Key << "Properties" << YAML::BeginSeq;
	for (prop::Property* pb : m_info.Properties) {
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << pb->getName();
		out << YAML::Key << "Type" << YAML::Value << static_cast<size_t>(pb->getType());
		out << YAML::Key << "Offset" << YAML::Value << pb->getOffset();
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	return true;
}

bool MaterialTypeAsset::deserialize(YAML::Node& data) {
	if (data["ElementCount"]) {
		m_info.ElementCount = data["ElementCount"].as<size_t>();
	}

	auto properties = data["Properties"];
	if (properties) {
		for (auto property : properties) {
			// Resolve type
			prop::PropertyType type = static_cast<prop::PropertyType>((property["Type"].as<size_t>()));
			std::string name = property["Name"].as<std::string>();
			uint16_t offset = property["Offset"].as<uint16_t>();

			prop::Property* propInstance = new prop::Property(type, name);
			propInstance->setOffset(offset);

			m_info.Properties.push_back(propInstance);
		}
	}

	return true;
}

ADERITE_ASSET_NAMESPACE_END
