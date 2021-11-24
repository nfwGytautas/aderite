#pragma once

#include <bgfx/bgfx.h>

#include "aderite/io/ILoadable.hpp"
#include "aderite/io/SerializableObject.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Texture asset implementation
 */
class TextureAsset final : public io::SerializableObject, public io::ILoadable {
public:
    /**
     * @brief Editable fields of the asset, this information is stored inside the asset file
     */
    struct fields {
        // TODO: Encapsulate
        bool IsHDR = false;
        bool IsCubemap = false;
    };

public:
    ~TextureAsset();

    /**
     * @brief Returns true if the texture is valid, false otherwise
     */
    bool isValid() const;

    // Inherited via ILoadable
    void load(const io::Loader* loader) override;
    void unload() override;
    bool needsLoading() const override;

    // Inherited via SerializableObject
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    /**
     * @brief Returns the info of shader fields
     */
    fields getFields() const;

    /**
     * @brief Returns mutable field structure
     */
    fields& getFieldsMutable();

    /**
     * @brief Returns texture handle of this asset
     */
    bgfx::TextureHandle getTextureHandle() const;

private:
    bgfx::TextureHandle m_handle = BGFX_INVALID_HANDLE;

    fields m_info = {};
};

} // namespace asset
} // namespace aderite
