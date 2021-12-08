#pragma once

#include "aderite/asset/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief A class that provides functionality for renderable objects
 */
class Renderable : public io::ISerializable {
public:
    virtual ~Renderable();

    /**
     * @brief Returns true if the renderable parameters are valid and it can be used for rendering
     */
    bool isValid() const;

    /**
     * @brief Computes a hash value of this renderable
     */
    size_t hash() const;

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
     * @brief Returns mesh instance
     */
    asset::MeshAsset* getMesh() const;

    /**
     * @brief Returns material instance
     */
    asset::MaterialAsset* getMaterial() const;

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    asset::MeshAsset* m_mesh = nullptr;
    asset::MaterialAsset* m_material = nullptr;
};

} // namespace rendering
} // namespace aderite
