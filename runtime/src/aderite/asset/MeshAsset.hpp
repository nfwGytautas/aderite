#pragma once

#include <bgfx/bgfx.h>

#include "aderite/io/Loader.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Mesh asset implementation
 */
class MeshAsset : public io::SerializableObject, public io::ILoadable {
public:
    /**
     * @brief Editable fields of the asset, this information is stored inside the asset file
     */
    struct fields {
        bool IsStatic = true;
    };

public:
    ~MeshAsset();

    /**
     * @brief Returns true if the mesh is valid
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
     * @brief Returns the info of mesh fields
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

    bgfx::VertexBufferHandle getVboHandle() const {
        return m_vbh;
    }

    bgfx::IndexBufferHandle getIboHandle() const {
        return m_ibh;
    }

private:
    // BGFX resource handles
    bgfx::VertexBufferHandle m_vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle m_ibh = BGFX_INVALID_HANDLE;

    fields m_info = {};
};

} // namespace asset
} // namespace aderite
