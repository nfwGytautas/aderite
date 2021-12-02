#pragma once

#include <bgfx/bgfx.h>

#include "aderite/io/ILoadable.hpp"
#include "aderite/io/SerializableAsset.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Mesh asset implementation
 */
class MeshAsset final : public io::SerializableAsset, public io::ILoadable {
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
    void load(const io::Loader* loader) override;
    void unload() override;
    bool needsLoading() const override;

    // Inherited via SerializableAsset
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    /**
     * @brief Returns the info of mesh fields
     */
    fields getFields() const;

    /**
     * @brief Returns mutable field structure
     */
    fields& getFieldsMutable();

    /**
     * @brief Returns the vertex buffer handle of the mesh
     */
    bgfx::VertexBufferHandle getVboHandle() const;

    /**
     * @brief Returns the index buffer handle of the mesh
     * @return
     */
    bgfx::IndexBufferHandle getIboHandle() const;

private:
    // BGFX resource handles
    bgfx::VertexBufferHandle m_vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle m_ibh = BGFX_INVALID_HANDLE;

    fields m_info = {};
};

} // namespace asset
} // namespace aderite
