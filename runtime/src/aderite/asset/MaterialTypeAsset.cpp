#include "MaterialTypeAsset.hpp"

namespace aderite {
namespace asset {

bgfx::ShaderHandle load_shader(const std::vector<unsigned char>& source, const std::string& name) {
	const bgfx::Memory* mem = bgfx::copy(source.data(), source.size() + 1);
	mem->data[mem->size - 1] = '\0';
	bgfx::ShaderHandle vsh = bgfx::createShader(mem);
	bgfx::setName(vsh, name.c_str());
	return vsh;
}

MaterialTypeAsset::~MaterialTypeAsset() {

}

void MaterialTypeAsset::load(const io::Loader* loader) {
	io::Loader::ShaderLoadResult slr = loader->loadShader(this->getHandle());
	if (!slr.Error.empty()) {
		return;
	}

	// Load bgfx bin shader
	bgfx::ShaderHandle vsh = load_shader(slr.VertexSource, this->getHandle() + " vertex");
	bgfx::ShaderHandle fsh = load_shader(slr.FragmentSource, this->getHandle() + " fragment");

	// Create program
	m_shaderHandle = bgfx::createProgram(vsh, fsh, true);

	// Create uniform
	const std::string typeName = std::to_string(this->getHandle());
	m_uniformHandle = bgfx::createUniform(
		("u_mat_buffer_" + typeName).c_str(),
		bgfx::UniformType::Vec4,
		m_info.Size);

	// Create samplers
	for (size_t i = 0; i < m_info.NumSamplers; i++) {
		m_samplers[i] = bgfx::createUniform(
			("s_" + typeName + "_" + std::to_string(i)).c_str(),
			bgfx::UniformType::Sampler,
			1);
	}
}

void MaterialTypeAsset::unload() {
	if (bgfx::isValid(m_shaderHandle)) {
		bgfx::destroy(m_shaderHandle);
		m_shaderHandle = BGFX_INVALID_HANDLE;
	}

	if (bgfx::isValid(m_uniformHandle)) {
		bgfx::destroy(m_uniformHandle);
		m_uniformHandle = BGFX_INVALID_HANDLE;
	}

	for (auto sampler : m_samplers) {
		if (bgfx::isValid(sampler)) {
			bgfx::destroy(sampler);
		}
	}

	m_samplers.clear();
}

reflection::Type MaterialTypeAsset::getType() const {
	return static_cast<reflection::Type>(reflection::RuntimeTypes::MAT_TYPE);
}

bool MaterialTypeAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	emitter << YAML::Key << "DataSize" << YAML::Value << m_info.Size;
	emitter << YAML::Key << "SamplerCount" << YAML::Value << m_info.NumSamplers;
	return true;
}

bool MaterialTypeAsset::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	m_info.Size = data["DataSize"].as<size_t>();
	m_info.NumSamplers = data["SamplerCount"].as<size_t>();
	return true;
}

}
}
