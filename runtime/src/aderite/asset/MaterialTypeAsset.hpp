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
    /**
     * @brief Editable fields of the asset, this information is stored inside the asset file
     */
    struct fields {
        size_t Size; // Number of vec4 components
        size_t NumSamplers;
        std::vector<std::string> SamplerNames;
    };

public:
    ADERITE_MIDDLEWARE_VIRTUAL ~MaterialTypeAsset();

    /**
     * @brief Returns true if the type is valid, false otherwise
     */
    bool isValid() const;

    // Inherited via SerializableAsset
    void load(const io::Loader* loader) override;
    void unload() override;
    bool needsLoading() const override;
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

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
     * @brief Returns the info of shader fields
     */
    fields getFields() const;

    /**
     * @brief Returns mutable field structure
     */
    fields& getFieldsMutable();

private:
    // Shader
    bgfx::ProgramHandle m_shaderHandle = BGFX_INVALID_HANDLE;

    // Material properties
    bgfx::UniformHandle m_uniformHandle = BGFX_INVALID_HANDLE;
    std::vector<bgfx::UniformHandle> m_samplers;

    fields m_info = {};
};

} // namespace asset
} // namespace aderite
