#pragma once

#include <string>
#include <vector>

#include <bgfx/bgfx.h>

#include "aderite/asset/Forward.hpp"
#include "aderite/io/ILoadable.hpp"
#include "aderite/io/SerializableAsset.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Material asset implementation
 */
class MaterialAsset final : public io::SerializableAsset, public io::ILoadable {
public:
    /**
     * @brief Editable fields of the asset, this information is stored inside the asset file
     */
    struct fields {
        MaterialTypeAsset* Type; // Material type
        std::vector<TextureAsset*> Samplers;
    };

public:
    ~MaterialAsset();

    /**
     * @brief Returns true if the material is valid and can be used for rendering
     */
    bool isValid() const;

    // Inherited via ILoadable
    void load(const io::Loader* loader) override;
    void unload() override;
    bool needsLoading() const override;

    // Inherited via SerializableAsset
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    /**
     * @brief Sets the type of the material
     * @param type New type
     */
    void setType(MaterialTypeAsset* type);

    /**
     * @brief Returns the info of shader fields
     */
    fields getFields() const;

    /**
     * @brief Returns mutable field structure
     */
    fields& getFieldsMutable();

    /**
     * @brief Returns material property data array
     */
    float* getPropertyData() const;

    /**
     * @brief Returns the sampler data vector
     */
    std::vector<std::pair<bgfx::UniformHandle, bgfx::TextureHandle>> getSamplerData() const;

private:
    fields m_info = {};
    float* m_udata = nullptr; // Data passed to material uniform
};

} // namespace asset
} // namespace aderite
