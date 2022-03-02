#include "MaterialTypeAsset.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

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
    LOG_TRACE("[Asset] Destroying {0}", this->getName());
    this->unload();
}

bool MaterialTypeAsset::isValid() const {
    for (bgfx::UniformHandle uh : m_samplers) {
        if (!bgfx::isValid(uh)) {
            return false;
        }
    }

    return bgfx::isValid(m_shaderHandle) && bgfx::isValid(m_uniformHandle);
}

void MaterialTypeAsset::load(const io::Loader* loader) {
    LOG_TRACE("[Asset] Loading {0}", this->getName());
    ADERITE_DYNAMIC_ASSERT(!bgfx::isValid(m_shaderHandle), "Tried to load already loaded material type");

    io::Loader::ShaderLoadResult slr = loader->loadShader(this->getHandle());
    if (!slr.Error.empty()) {
        return;
    }

    if (slr.FragmentSource.size() == 0 || slr.VertexSource.size() == 0) {
        // Something is wrong, try again another time
        return;
    }

    // Load bgfx bin shader
    bgfx::ShaderHandle vsh = load_shader(slr.VertexSource, this->getName() + " vertex");
    bgfx::ShaderHandle fsh = load_shader(slr.FragmentSource, this->getName() + " fragment");

    // Create program
    m_shaderHandle = bgfx::createProgram(vsh, fsh, true);

    // Create uniform
    m_uniformHandle = bgfx::createUniform(("mf_mat_buffer_" + this->getName()).c_str(), bgfx::UniformType::Vec4, m_info.Size);

    // Create samplers
    m_samplers.resize(m_info.NumSamplers);
    for (size_t i = 0; i < m_info.NumSamplers; i++) {
        m_samplers[i] =
            bgfx::createUniform(("mf_" + this->getName() + "_" + m_info.SamplerNames[i]).c_str(), bgfx::UniformType::Sampler, 1);
    }

    LOG_INFO("[Asset] Loaded {0}", this->getName());
}

void MaterialTypeAsset::unload() {
    LOG_TRACE("[Asset] Unloading {0}", this->getName());

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

    LOG_INFO("[Asset] Unloaded {0}", this->getName());
}

bool MaterialTypeAsset::needsLoading() const {
    // Check if sources exist
    if (!::aderite::Engine::getFileHandler()->exists(this->getHandle())) {
        return false;
    }

    return !this->isValid();
}

reflection::Type MaterialTypeAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::MAT_TYPE);
}

bool MaterialTypeAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "DataSize" << YAML::Value << m_info.Size;
    emitter << YAML::Key << "SamplerCount" << YAML::Value << m_info.NumSamplers;
    emitter << YAML::Key << "SamplerNames" << YAML::Flow << YAML::BeginSeq;
    for (const std::string& name : m_info.SamplerNames) {
        emitter << name;
    }
    emitter << YAML::EndSeq;
    return true;
}

bool MaterialTypeAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_info.Size = data["DataSize"].as<size_t>();
    m_info.NumSamplers = data["SamplerCount"].as<size_t>();
    for (const YAML::Node& samplerName : data["SamplerNames"]) {
        m_info.SamplerNames.push_back(samplerName.as<std::string>());
    }
    return true;
}

bgfx::ProgramHandle MaterialTypeAsset::getShaderHandle() const {
    return m_shaderHandle;
}

bgfx::UniformHandle MaterialTypeAsset::getUniformHandle() const {
    return m_uniformHandle;
}

bgfx::UniformHandle MaterialTypeAsset::getSampler(size_t idx) const {
    return m_samplers[idx];
}

MaterialTypeAsset::fields MaterialTypeAsset::getFields() const {
    return m_info;
}

MaterialTypeAsset::fields& MaterialTypeAsset::getFieldsMutable() {
    return m_info;
}

} // namespace asset
} // namespace aderite
