#pragma once

#include <vector>

#include <bgfx/bgfx.h>

#include "aderite/io/SerializableAsset.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Material type asset, is a collection of a shader, data uniform and samplers
 */
class MaterialTypeAsset ADERITE_MIDDLEWARE_FINAL : public io::SerializableAsset {
public:
    ADERITE_MIDDLEWARE_VIRTUAL ~MaterialTypeAsset();

    /**
     * @brief Get the shader handle of the material type
     */
    bgfx::ProgramHandle getShaderHandle() const;

    /**
     * @brief Get the uniform handle of the material type
     */
    bgfx::UniformHandle getUniformHandle() const;

    /**
     * @brief Get sampler at the specified index
     * @param idx Sampler index
     */
    bgfx::UniformHandle getSampler(size_t idx) const;

    /**
     * @brief Returns true if the type is valid, false otherwise
     */
    bool isValid() const;

    /**
     * @brief Returns the size of this material type in number of vec4 components
     */
    size_t getSize() const;

    /**
     * @brief Set the size of the material type
     * @param value Number of vec4 components
     */
    void setSize(size_t value);

    /**
     * @brief Returns the number of samplers in this material type
     */
    size_t getSamplerCount() const;

    /**
     * @brief Sets the number of samplers in this material type
     * @param count Number of samplers
     */
    void setSamplerCount(size_t count);

    /**
     * @brief Returns the sampler names of this material type
     */
    std::vector<std::string> getSamplerNames() const;

    /**
     * @brief Sets the samplers names of this material type, must be the same as the sampler count
     * @param values Names list
     */
    void setSamplerNames(const std::vector<std::string>& values);

    // Inherited via SerializableAsset
    void load(const io::Loader* loader) override;
    void unload() override;
    bool needsLoading() const override;
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    // Shader
    bgfx::ProgramHandle m_shaderHandle = BGFX_INVALID_HANDLE;

    // Material properties
    bgfx::UniformHandle m_uniformHandle = BGFX_INVALID_HANDLE;
    std::vector<bgfx::UniformHandle> m_samplers;

    size_t m_size; // Number of vec4 components
    size_t m_numSamplers;
    std::vector<std::string> m_samplerNames;
};

} // namespace asset
} // namespace aderite
