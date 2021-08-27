#include "ShaderAsset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/rendering/DrawCall.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

bgfx::ShaderHandle load_shader(const std::vector<unsigned char>& source, const std::string& name) {
	const bgfx::Memory* mem = bgfx::copy(source.data(), source.size() + 1);
	mem->data[mem->size - 1] = '\0';
	bgfx::ShaderHandle vsh = bgfx::createShader(mem);
	bgfx::setName(vsh, name.c_str());
	return vsh;
}

ShaderAsset::~ShaderAsset() {
	if (bgfx::isValid(m_handle)) {
		LOG_WARN("Deleting a loaded shader asset {0}", getName());
	}
}

AssetType ShaderAsset::type() const {
	return AssetType::SHADER;
}

bool ShaderAsset::serialize(YAML::Emitter& out) {
	// Shader
	out << YAML::Key << "Vertex" << YAML::Value << m_info.VertexPath;
	out << YAML::Key << "Fragment" << YAML::Value << m_info.FragmentPath;

	return true;
}

bool ShaderAsset::deserialize(YAML::Node& data) {
	// TODO: Error check
	m_info.VertexPath = data["Vertex"].as<std::string>();
	m_info.FragmentPath = data["Fragment"].as<std::string>();

	return true;
}

void ShaderAsset::fillDrawCall(rendering::DrawCall* dc) {
	if (!isLoaded()) {
		dc->Valid = false;
		return;
	}

	dc->Shader = m_handle;
}

void ShaderAsset::load() {
	if (isLoaded()) {
		LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
		unload();
	}

	// Load bgfx bin shader
	bgfx::ShaderHandle vsh = load_shader(m_vertexSource, "vVertex");
	bgfx::ShaderHandle fsh = load_shader(m_fragmentSource, "fVertex");

	// Create program
	m_handle = bgfx::createProgram(vsh, fsh, true);

	m_isBeingPrepared = false;
}

void ShaderAsset::unload() {
	if (bgfx::isValid(m_handle)) {
		bgfx::destroy(m_handle);
		m_handle = BGFX_INVALID_HANDLE;
	}
}

bool ShaderAsset::isPreparing() {
	return m_isBeingPrepared;
}

bool ShaderAsset::isLoaded() {
	return bgfx::isValid(m_handle);
}

size_t ShaderAsset::hash() const {
	return std::hash<std::string>{}(p_name);
}

ShaderAsset::ShaderAsset(const std::string& name)
	: Asset(name)
{}

ShaderAsset::ShaderAsset(const std::string& name, const fields& info)
	: Asset(name), m_info(info)
{}

bool ShaderAsset::isInGroup(AssetGroup group) const {
	switch (group) {
	case AssetGroup::DEPENDS_ON_RAW:
	{
		return true;
	}
	default:
		return false;
	}
}

void ShaderAsset::prepareLoad() {
	// Load sources
	// TODO: Async
	m_vertexSource = ::aderite::Engine::getAssetManager()->loadBinFile(m_info.VertexPath);
	m_fragmentSource = ::aderite::Engine::getAssetManager()->loadBinFile(m_info.FragmentPath);
	m_isBeingPrepared = true;
}

bool ShaderAsset::isReadyToLoad() {
	return (m_vertexSource.size() > 0 && m_fragmentSource.size() > 0);
}

ADERITE_ASSET_NAMESPACE_END
