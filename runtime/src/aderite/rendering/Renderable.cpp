#include "Renderable.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/io/Serializer.hpp"

namespace aderite {
namespace rendering {

void Renderable::setMesh(asset::MeshAsset* mesh) {
    m_meshHandle = mesh;
}

void Renderable::setMaterial(asset::MaterialAsset* material) {
    m_materialHandle = material;
}

DrawCall Renderable::createDrawCall() const {
    return DrawCall {m_meshHandle->getVboHandle(),
                     m_meshHandle->getIboHandle(),
                     m_materialHandle->getFields().Type->getShaderHandle(),
                     m_materialHandle->getFields().Type->getUniformHandle(),
                     m_materialHandle->getSamplerData(),
                     m_materialHandle->getPropertyData(),
                     std::vector<glm::mat4>()};
}

bool Renderable::isValid() const {
    return m_meshHandle != nullptr && m_materialHandle != nullptr && m_meshHandle->isValid() && m_materialHandle->isValid();
}

void Renderable::loadIfNeeded() {
    if (!m_meshHandle->isValid()) {
        ::aderite::Engine::getLoaderPool()->enqueue(m_meshHandle, io::LoaderPool::Priority::HIGH);
    }

    if (!m_materialHandle->isValid()) {
        ::aderite::Engine::getLoaderPool()->enqueue(m_materialHandle, io::LoaderPool::Priority::HIGH);
    }
}

reflection::Type Renderable::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::RENDERABLE);
}

bool Renderable::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (m_meshHandle) {
        emitter << YAML::Key << "Mesh" << YAML::Value << m_meshHandle->getHandle();
    }

    if (m_materialHandle) {
        emitter << YAML::Key << "Material" << YAML::Value << m_materialHandle->getHandle();
    }

    return true;
}

bool Renderable::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (data["Mesh"]) {
        const io::SerializableHandle handle = data["Mesh"].as<io::SerializableHandle>();
        m_meshHandle = static_cast<asset::MeshAsset*>(serializer->getOrRead(handle));
        ADERITE_DYNAMIC_ASSERT(m_meshHandle != nullptr, "Tried to use a deleted asset");
    }

    if (data["Material"]) {
        const io::SerializableHandle handle = data["Material"].as<io::SerializableHandle>();
        m_materialHandle = static_cast<asset::MaterialAsset*>(serializer->getOrRead(handle));
        ADERITE_DYNAMIC_ASSERT(m_materialHandle != nullptr, "Tried to use a deleted asset");
    }

    return true;
}

} // namespace rendering
} // namespace aderite
