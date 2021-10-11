#include "MaterialTypeAsset.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/utility/Log.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

bgfx::ShaderHandle load_shader(const std::vector<unsigned char>& source, const std::string& name) {
	const bgfx::Memory* mem = bgfx::copy(source.data(), source.size() + 1);
	mem->data[mem->size - 1] = '\0';
	bgfx::ShaderHandle vsh = bgfx::createShader(mem);
	bgfx::setName(vsh, name.c_str());
	return vsh;
}

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
	// Load sources
	// TODO: Async
	std::string typeName = getName();
	std::replace_if(std::begin(typeName), std::end(typeName),
		[](std::string::value_type v) { return v == '.'; },
		'_');

	m_vertexSource = ::aderite::Engine::getAssetManager()->loadBinFile("vs_" + typeName + ".bin");
	m_fragmentSource = ::aderite::Engine::getAssetManager()->loadBinFile("fs_" + typeName + ".bin");

	if (m_vertexSource.size() == 0 || m_fragmentSource.size() == 0) {
		// This is to not block when there are no sources, will be reworked together with entire asset system
		m_vertexSource.push_back('_');
		m_fragmentSource.push_back('_');
	}

	m_isBeingPrepared = true;
}

bool MaterialTypeAsset::isReadyToLoad() {
	return (m_vertexSource.size() > 0 && m_fragmentSource.size() > 0);
}

void MaterialTypeAsset::load() {
	if (isLoaded()) {
		LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
		unload();
	}

	if (m_vertexSource.size() == 1 || m_fragmentSource.size() == 1) {
		LOG_WARN("Failed to load");
		return;
	}

	// Load bgfx bin shader
	bgfx::ShaderHandle vsh = load_shader(m_vertexSource, "vVertex");
	bgfx::ShaderHandle fsh = load_shader(m_fragmentSource, "fVertex");

	// Create program
	m_handle = bgfx::createProgram(vsh, fsh, true);

	// Create uniform
	std::string typeName = getName();
	std::replace_if(std::begin(typeName), std::end(typeName),
		[](std::string::value_type v) { return v == '.'; },
		'_');
	m_uniform = bgfx::createUniform(
		("u_mat_buffer_" + typeName).c_str(), 
		bgfx::UniformType::Vec4,
		m_info.v4Count);

	// Create samplers
	for (prop::Property* prop : m_info.Properties) {
		if (prop::isSampler(prop->getType())) {
			m_samplers[prop->getName()] = bgfx::createUniform(
				("s_" + typeName + "_" + prop->getName()).c_str(), 
				bgfx::UniformType::Sampler, 
				1);
		}
	}

	m_isBeingPrepared = false;
}

void MaterialTypeAsset::unload() {
	if (bgfx::isValid(m_handle)) {
		bgfx::destroy(m_handle);
		m_handle = BGFX_INVALID_HANDLE;
	}

	if (bgfx::isValid(m_uniform)) {
		bgfx::destroy(m_uniform);
		m_uniform = BGFX_INVALID_HANDLE;
	}

	for (auto handle : m_samplers) {
		if (bgfx::isValid(handle.second)) {
			bgfx::destroy(handle.second);
		}
	}

	m_samplers.clear();
}

bool MaterialTypeAsset::isPreparing() {
	return m_isBeingPrepared;
}

bool MaterialTypeAsset::isLoaded() {
	return bgfx::isValid(m_handle);;
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

	if (m_info.ElementCount % 4 != 0) {
		m_info.v4Count = ((m_info.ElementCount - m_info.ElementCount % 4) / 4) + 1;
	}
	else {
		m_info.v4Count = m_info.ElementCount / 4;
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
