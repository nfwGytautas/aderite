#include "ShaderAsset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"
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

void ShaderAsset::fill_draw_call(rendering::DrawCall* dc) {
	dc->Shader = m_handle;
}

void ShaderAsset::load() {
	if (is_loaded()) {
		LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
		unload();
	}

	// Load bgfx bin shader
	bgfx::ShaderHandle vsh = load_shader(m_vertexSource, "vVertex");
	bgfx::ShaderHandle fsh = load_shader(m_fragmentSource, "fVertex");

	// Create program
	m_handle = bgfx::createProgram(vsh, fsh, true);

	m_being_prepared = false;
}

void ShaderAsset::unload() {
	if (bgfx::isValid(m_handle)) {
		bgfx::destroy(m_handle);
		m_handle = BGFX_INVALID_HANDLE;
	}
}

bool ShaderAsset::is_preparing() {
	return m_being_prepared;
}

bool ShaderAsset::is_loaded() {
	return bgfx::isValid(m_handle);
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

void ShaderAsset::prepare_load() {
	// Load sources
	// TODO: Async
	m_vertexSource = engine::get_AssetManager()->load_bin_file(m_info.VertexPath);
	m_fragmentSource = engine::get_AssetManager()->load_bin_file(m_info.FragmentPath);
	m_being_prepared = true;
}

bool ShaderAsset::ready_to_load() {
	return (m_vertexSource.size() > 0 && m_fragmentSource.size() > 0);
}

ADERITE_ASSET_NAMESPACE_END
