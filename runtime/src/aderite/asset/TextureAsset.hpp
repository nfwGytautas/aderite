#pragma once

#include <bgfx/bgfx.h>

#include "aderite/io/SerializableAsset.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Texture asset implementation
 */
class TextureAsset final : public io::SerializableAsset {
public:
    ~TextureAsset();

    /**
     * @brief Set the HDR format value, will need to reload
     * @param value If true then the texture is set to HDR format, if false then to normal format
     */
    void setHDR(bool value);

    /**
     * @brief Returns true if the texture is HDR format, false otherwise
     */
    bool isHDR() const;

    /**
     * @brief Set the cubemap format value, will need to reload
     * @param value If true then the texture is set to cubemap format, if false then to single image format
     */
    void setCubemap(bool value);

    /**
     * @brief Returns true if the texture is cubemap format, false otherwise
     */
    bool isCubemap() const;

    /**
     * @brief Returns texture handle of this asset
     */
    bgfx::TextureHandle getTextureHandle() const;

    /**
     * @brief Returns true if the texture is valid, false otherwise
     */
    bool isValid() const;

    // Inherited via SerializableAsset
    void load(const io::Loader* loader) override;
    void unload() override;
    bool needsLoading() const override;
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    bgfx::TextureHandle m_handle = BGFX_INVALID_HANDLE;

    /**
     * @brief If true then the texture data is treated as floating point instead of unsigned int
     */
    bool m_isHDR = false;

    /**
     * @brief If true then the data is treated as if it has 6 sides
     */
    bool m_isCubemap = false;
};

} // namespace asset
} // namespace aderite
