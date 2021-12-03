#include "MaterialAsset.hpp"

#include <bgfx/bgfx.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace asset {

MaterialAsset::~MaterialAsset() {
    LOG_TRACE("[Asset] Destroying {0}", this->getHandle());
    std::free(m_udata);
}

bool MaterialAsset::isValid() const {
    if (m_info.Type == nullptr || !m_info.Type->isValid()) {
        return false;
    }

    for (asset::TextureAsset* ta : m_info.Samplers) {
        if (ta == nullptr || !ta->isValid()) {
            return false;
        }
    }

    return true;
}

void MaterialAsset::load(const io::Loader* loader) {
    LOG_TRACE("[Asset] Loading {0}", this->getHandle());

    // TODO: Reference

    ::aderite::Engine::getLoaderPool()->enqueue(m_info.Type, io::LoaderPool::Priority::HIGH);

    for (asset::TextureAsset* ta : m_info.Samplers) {
        if (ta != nullptr && !ta->isValid()) {
            ::aderite::Engine::getLoaderPool()->enqueue(ta, io::LoaderPool::Priority::HIGH);
        }
    }

    LOG_INFO("[Asset] Loaded {0}", this->getHandle());
}

void MaterialAsset::unload() {
    LOG_TRACE("[Asset] Unloading {0}", this->getHandle());

    // TODO: Rework cause this should be reference counted

    LOG_INFO("[Asset] Unloaded {0}", this->getHandle());
}

bool MaterialAsset::needsLoading() const {
    return !this->isValid();
}

reflection::Type MaterialAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::MATERIAL);
}

bool MaterialAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Material
    if (m_info.Type == nullptr) {
        // Can't serialize anything
        return true;
    }

    emitter << YAML::Key << "MaterialType" << YAML::Value << m_info.Type->getHandle();
    emitter << YAML::Key << "Properties" << YAML::BeginMap;
    emitter << YAML::Key << "Data" << YAML::Flow << YAML::BeginSeq;
    for (size_t i = 0; i < m_info.Type->getFields().Size * 4; i++) {
        float val = m_udata[i];
        emitter << m_udata[i];
    }
    emitter << YAML::EndSeq;
    emitter << YAML::EndMap;

    emitter << YAML::Key << "Samplers" << YAML::Flow << YAML::BeginSeq;
    for (auto sampler : m_info.Samplers) {
        if (sampler != nullptr) {
            emitter << sampler->getHandle();
        } else {
            emitter << YAML::Null;
        }
    }
    emitter << YAML::EndSeq;

    return true;
}

bool MaterialAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!data["MaterialType"]) {
        return true;
    }
    // TODO: Error check
    io::SerializableHandle typeName = data["MaterialType"].as<io::SerializableHandle>();
    m_info.Type = static_cast<MaterialTypeAsset*>(::aderite::Engine::getAssetManager()->get(typeName));
    const size_t dataSize = m_info.Type->getFields().Size * 4 * sizeof(float);
    m_udata = static_cast<float*>(std::malloc(dataSize));

    if (m_udata == nullptr) {
        LOG_ERROR("[Asset] Failed to allocate space for material property data {0}", getHandle());
        return false;
    }

    std::memset(m_udata, 0, dataSize);

    const YAML::Node& d = data["Properties"]["Data"];

    if (d.size() != (m_info.Type->getFields().Size * 4)) {
        LOG_ERROR("[Asset] Incorrect size for stored material data and type {0}", getHandle());
        return false;
    }

    for (size_t i = 0; i < d.size(); i++) {
        m_udata[i] = d[i].as<float>();
    }

    for (const YAML::Node& sampler : data["Samplers"]) {
        TextureAsset* texture = nullptr;

        if (!sampler.IsNull()) {
            m_info.Samplers.push_back(
                static_cast<TextureAsset*>(::aderite::Engine::getAssetManager()->get(sampler.as<io::SerializableHandle>())));
        } else {
            m_info.Samplers.push_back(nullptr);
        }
    }

    return true;
}

void MaterialAsset::setType(MaterialTypeAsset* type) {
    // TODO: Move to editor
    m_info.Type = type;
    const size_t dataSize = m_info.Type->getFields().Size * 4 * sizeof(float);

    // Samplers
    m_info.Samplers.clear();

    float* realloced = static_cast<float*>(std::realloc(m_udata, dataSize));
    if (realloced != nullptr) {
        m_udata = realloced;
    } else {
        LOG_ERROR("[Asset] Failed to realloc material {0} data array", this->getHandle());
        return;
    }

    std::memset(m_udata, 0, dataSize);

    // TODO: Dereference textures
    for (size_t i = 0; i < m_info.Type->getFields().NumSamplers; i++) {
        m_info.Samplers.push_back(nullptr);
    }
}

MaterialAsset::fields MaterialAsset::getFields() const {
    return m_info;
}

MaterialAsset::fields& MaterialAsset::getFieldsMutable() {
    return m_info;
}

float* MaterialAsset::getPropertyData() const {
    return m_udata;
}

std::vector<std::pair<bgfx::UniformHandle, bgfx::TextureHandle>> MaterialAsset::getSamplerData() const {
    std::vector<std::pair<bgfx::UniformHandle, bgfx::TextureHandle>> result;

    for (size_t i = 0; i < m_info.Type->getFields().NumSamplers; i++) {
        if (m_info.Samplers[i] == nullptr) {
            result.push_back(std::make_pair(m_info.Type->getSampler(i), bgfx::TextureHandle {bgfx::kInvalidHandle}));
        } else {
            result.push_back(std::make_pair(m_info.Type->getSampler(i), m_info.Samplers[i]->getTextureHandle()));
        }
    }

    return result;
}

} // namespace asset
} // namespace aderite
