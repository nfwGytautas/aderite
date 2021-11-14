#pragma once

#include "aderite/asset/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/rendering/DrawCall.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Renderable class that contains data needed to render an object, including mesh, material and other rendering data
 */
class Renderable final : public io::ISerializable {
public:
    /**
     * @brief Set the mesh of the renderable
     * @param mesh Mesh to set
     */
    void setMesh(asset::MeshAsset* mesh);

    /**
     * @brief Set the material of the renderable
     * @param material Material to set
     */
    void setMaterial(asset::MaterialAsset* material);

    /**
     * @brief Creates and returns a DrawCall object from this renderable
     */
    DrawCall createDrawCall() const;

    /**
     * @brief Returns mesh instance
     */
    asset::MeshAsset* getMesh() const;

    /**
     * @brief Returns material instance
     */
    asset::MaterialAsset* getMaterial() const;

    /**
     * @brief Returns true if the renderable is valid and can be used to render, false otherwise
     */
    bool isValid() const;

    /**
     * @brief Loads the renderable assets if needed
     */
    void loadIfNeeded();

    // Inherited via ISerializable
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    asset::MeshAsset* m_meshHandle = nullptr;
    asset::MaterialAsset* m_materialHandle = nullptr;
};

} // namespace rendering
} // namespace aderite
