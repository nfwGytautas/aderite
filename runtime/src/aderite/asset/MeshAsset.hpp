#pragma once

#include <bgfx/bgfx.h>

#include "aderite/io/SerializableAsset.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Mesh asset implementation
 */
class MeshAsset final : public io::SerializableAsset {
public:
    ~MeshAsset();

    /**
     * @brief Returns the vertex buffer handle of the mesh
     */
    bgfx::VertexBufferHandle getVboHandle() const;

    /**
     * @brief Returns the index buffer handle of the mesh
     * @return
     */
    bgfx::IndexBufferHandle getIboHandle() const;

    /**
     * @brief Returns true if the mesh is valid
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
    // BGFX resource handles
    bgfx::VertexBufferHandle m_vbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle m_ibh = BGFX_INVALID_HANDLE;
};

} // namespace asset
} // namespace aderite
