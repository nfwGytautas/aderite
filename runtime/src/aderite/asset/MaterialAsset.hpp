#pragma once

#include <string>
#include <vector>

#include <bgfx/bgfx.h>

#include "aderite/asset/Forward.hpp"
#include "aderite/io/SerializableAsset.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Material asset implementation
 */
class MaterialAsset final : public io::SerializableAsset {
public:
    ~MaterialAsset();

    /**
     * @brief Returns the type of this material
     */
    MaterialTypeAsset* getMaterialType() const;

    /**
     * @brief Set the type of the material asset
     * @param type New type of the material
     */
    void setMaterialType(MaterialTypeAsset* type);

    /**
     * @brief Returns true if the material is valid and can be used for rendering
     */
    bool isValid() const;

    /**
     * @brief Set the sampler
     * @param index Index of the sampler
     * @param ta Texture asset
     */
    void setSampler(size_t index, TextureAsset* ta);

    /**
     * @brief Returns the number of samplers in the material
     */
    size_t getSamplerCount() const;

    /**
     * @brief Returns the sampler at the specified index
     * @param index Index of the sampler
     * @return TextureAsset instance
     */
    TextureAsset* getSampler(size_t index) const;

    /**
     * @brief Returns material property data array
     */
    float* getPropertyData() const;

    /**
     * @brief Returns the sampler data vector
     */
    //std::vector<std::pair<bgfx::UniformHandle, bgfx::TextureHandle>> getSamplerData() const;

    // Inherited via SerializableAsset
    void load(const io::Loader* loader) override;
    void unload() override;
    bool needsLoading() const override;
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    MaterialTypeAsset* m_type = nullptr; // Material type
    std::vector<TextureAsset*> m_samplers;
    float* m_udata = nullptr; // Data passed to material uniform
};

} // namespace asset
} // namespace aderite
