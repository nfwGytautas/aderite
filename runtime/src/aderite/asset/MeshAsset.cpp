#include "MeshAsset.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace asset {

MeshAsset::~MeshAsset() {
    LOG_TRACE("[Asset] Destroying {0}", this->getName());

    if (bgfx::isValid(m_vbh) || bgfx::isValid(m_ibh)) {
        this->unload();
    }
}

bool MeshAsset::isValid() const {
    return bgfx::isValid(m_vbh) && bgfx::isValid(m_ibh);
}

void MeshAsset::load(const io::Loader* loader) {
    LOG_TRACE("[Asset] Loading {0}", this->getName());
    ADERITE_DYNAMIC_ASSERT(!bgfx::isValid(m_vbh), "Tried to load already loaded mesh");

    // Create layout
    bgfx::VertexLayout layout;
    layout.begin();
    layout.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
    layout.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float);
    layout.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
    layout.end();

    // Create handles
    if (m_info.IsStatic) {
        io::Loader::MeshLoadResult result = loader->loadMesh(this->getHandle());
        if (!result.Error.empty()) {
            LOG_WARN("[Asset] Mesh load error: {0}", result.Error);
            return;
        }

        auto& positionData = result.Vertices;
        auto& indicesData = result.Indices;
        m_vbh = bgfx::createVertexBuffer(bgfx::copy(positionData.data(), sizeof(float) * positionData.size()), layout);
        m_ibh = bgfx::createIndexBuffer(bgfx::copy(indicesData.data(), sizeof(unsigned int) * indicesData.size()), BGFX_BUFFER_INDEX32);

        bgfx::setName(m_vbh, this->getName().c_str());
        bgfx::setName(m_ibh, this->getName().c_str());
    } else {
        LOG_ERROR("[Asset] Unimplemented dynamic mesh");
        return;
    }

    LOG_INFO("[Asset] Loaded {0}", this->getName());
}

void MeshAsset::unload() {
    LOG_TRACE("[Asset] Unloading {0}", this->getName());

    if (bgfx::isValid(m_vbh)) {
        bgfx::destroy(m_vbh);
        m_vbh = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(m_ibh)) {
        bgfx::destroy(m_ibh);
        m_ibh = BGFX_INVALID_HANDLE;
    }

    LOG_INFO("[Asset] Unloaded {0}", this->getName());
}

bool MeshAsset::needsLoading() const {
    return !this->isValid();
}

reflection::Type MeshAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::MESH);
}

bool MeshAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Layout
    emitter << YAML::Key << "IsStatic" << YAML::Value << m_info.IsStatic;
    return true;
}

bool MeshAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_info.IsStatic = data["IsStatic"].as<bool>();
    return true;
}

MeshAsset::fields MeshAsset::getFields() const {
    return m_info;
}

MeshAsset::fields& MeshAsset::getFieldsMutable() {
    return m_info;
}

bgfx::VertexBufferHandle MeshAsset::getVboHandle() const {
    return m_vbh;
}

bgfx::IndexBufferHandle MeshAsset::getIboHandle() const {
    return m_ibh;
}

} // namespace asset
} // namespace aderite
