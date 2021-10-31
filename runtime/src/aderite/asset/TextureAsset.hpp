#pragma once

#include <bgfx/bgfx.h>

#include "aderite/Handles.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Texture asset implementation
 */
class TextureAsset : public io::SerializableObject, public io::ILoadable {
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
    virtual void load(const io::Loader* loader) override;
    virtual void unload() override;
    virtual bool needsLoading() override;

    // Inherited via SerializableObject
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    /**
     * @brief Returns the info of shader fields
     */
    fields getFields() const {
        return m_info;
    }

    /**
     * @brief Returns mutable field structure
     */
    fields& getFieldsMutable() {
        return m_info;
    }

    /**
     * @brief Returns texture handle of this asset
     */
    bgfx::TextureHandle getTextureHandle() const {
        return m_handle;
    }

private:
    bgfx::TextureHandle m_handle = BGFX_INVALID_HANDLE;

    fields m_info = {};
};

} // namespace asset
} // namespace aderite
