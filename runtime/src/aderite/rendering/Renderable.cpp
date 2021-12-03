#include "Renderable.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace rendering {

void Renderable::setMesh(asset::MeshAsset* mesh) {
    m_mesh = mesh;
}

void Renderable::setMaterial(asset::MaterialAsset* material) {
    m_material = material;
}

asset::MeshAsset* Renderable::getMesh() const {
    return m_mesh;
}

asset::MaterialAsset* Renderable::getMaterial() const {
    return m_material;
}

bool Renderable::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Renderable" << YAML::BeginMap;
    if (m_mesh) {
        emitter << YAML::Key << "Mesh" << YAML::Value << m_mesh->getHandle();
    }

    if (m_material) {
        emitter << YAML::Key << "Material" << YAML::Value << m_material->getHandle();
    }
    emitter << YAML::EndMap;

    return true;
}

bool Renderable::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& renderNode = data["Renderable"];
    if (!renderNode || renderNode.IsNull()) {
        return false;
    }

    if (renderNode["Mesh"]) {
        const io::SerializableHandle handle = data["Mesh"].as<io::SerializableHandle>();
        m_mesh = static_cast<asset::MeshAsset*>(::aderite::Engine::getAssetManager()->get(handle));
        ADERITE_DYNAMIC_ASSERT(m_mesh != nullptr, "Tried to use a deleted asset");
    }

    if (renderNode["Material"]) {
        const io::SerializableHandle handle = data["Material"].as<io::SerializableHandle>();
        m_material = static_cast<asset::MaterialAsset*>(::aderite::Engine::getAssetManager()->get(handle));
        ADERITE_DYNAMIC_ASSERT(m_material != nullptr, "Tried to use a deleted asset");
    }

    return true;
}

} // namespace rendering
} // namespace aderite
