#include "MeshAsset.hpp"
#include <fstream>

#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace asset {

MeshAsset::~MeshAsset() {
    // TODO: Check for either handle to be valid
    if (bgfx::isValid(m_vbh)) {
        LOG_WARN("Deleting a loaded mesh asset {0}", getHandle());
    }
}

bool MeshAsset::isValid() const {
    return bgfx::isValid(m_vbh) && bgfx::isValid(m_ibh);
}

void MeshAsset::load(const io::Loader* loader) {
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
            return;
        }

        auto& positionData = result.Vertices;
        auto& indicesData = result.Indices;
        m_vbh = bgfx::createVertexBuffer(bgfx::copy(positionData.data(), sizeof(float) * positionData.size()), layout);
        m_ibh = bgfx::createIndexBuffer(bgfx::copy(indicesData.data(), sizeof(unsigned int) * indicesData.size()), BGFX_BUFFER_INDEX32);
    } else {
        LOG_ERROR("Unimplemented dynamic mesh");
        return;
    }
}

void MeshAsset::unload() {
    if (bgfx::isValid(m_vbh)) {
        bgfx::destroy(m_vbh);
        m_vbh = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(m_ibh)) {
        bgfx::destroy(m_ibh);
        m_ibh = BGFX_INVALID_HANDLE;
    }
}

bool MeshAsset::needsLoading() {
    return !this->isValid();
}

reflection::Type MeshAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::MESH);
}

bool MeshAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    // Layout
    emitter << YAML::Key << "IsStatic" << YAML::Value << m_info.IsStatic;
    return true;
}

bool MeshAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_info.IsStatic = data["IsStatic"].as<bool>();
    return true;
}

} // namespace asset
} // namespace aderite
