#include "RenderableData.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Utility.hpp"

namespace aderite {
namespace rendering {

RenderableData::~RenderableData() {
    if (m_mesh != nullptr) {
        m_mesh->release();
    }

    if (m_material != nullptr) {
        m_material->release();
    }
}

bool RenderableData::isValid() const {
    return m_mesh != nullptr && m_material != nullptr && m_mesh->isValid() && m_material->isValid();
}

size_t RenderableData::hash() const {
    ADERITE_DYNAMIC_ASSERT(this->isValid(), "Tried to get hash of invalid renderable");
    return utility::combineHash(m_mesh->getHandle(), m_material->getHandle());
}

void RenderableData::setMesh(asset::MeshAsset* mesh) {
    if (m_mesh != nullptr) {
        m_mesh->release();
    }

    m_mesh = mesh;
    m_mesh->acquire();
}

void RenderableData::setMaterial(asset::MaterialAsset* material) {
    if (m_material != nullptr) {
        m_material->release();
    }

    m_material = material;
    m_material->acquire();
}

asset::MeshAsset* RenderableData::getMesh() const {
    return m_mesh;
}

asset::MaterialAsset* RenderableData::getMaterial() const {
    return m_material;
}

bool RenderableData::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
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

bool RenderableData::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& renderNode = data["Renderable"];
    if (!renderNode || renderNode.IsNull()) {
        return false;
    }

    if (renderNode["Mesh"]) {
        const io::SerializableHandle handle = renderNode["Mesh"].as<io::SerializableHandle>();
        this->setMesh(static_cast<asset::MeshAsset*>(::aderite::Engine::getAssetManager()->get(handle)));
    }

    if (renderNode["Material"]) {
        const io::SerializableHandle handle = renderNode["Material"].as<io::SerializableHandle>();
        this->setMaterial(static_cast<asset::MaterialAsset*>(::aderite::Engine::getAssetManager()->get(handle)));
    }

    return true;
}

RenderableData& RenderableData::operator=(const RenderableData& other) {
    this->setMaterial(other.getMaterial());
    this->setMesh(other.getMesh());
    return *this;
}

} // namespace rendering
} // namespace aderite
