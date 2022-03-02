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
    LOG_TRACE("[Asset] Destroying {0}", this->getName());
    std::free(m_udata);

    if (m_type != nullptr) {
        m_type->release();
    }

    for (TextureAsset* ta : m_samplers) {
        if (ta != nullptr) {
            ta->release();
        }
    }
}

MaterialTypeAsset* MaterialAsset::getMaterialType() const {
    return m_type;
}

void MaterialAsset::setMaterialType(MaterialTypeAsset* type) {
    if (m_type != nullptr) {
        m_type->release();
    }

    m_type = type;
    m_type->acquire();

    // Samplers
    const size_t dataSize = m_type->getFields().Size * 4 * sizeof(float);

    // Release previous references
    for (TextureAsset* ta : m_samplers) {
        if (ta != nullptr) {
            ta->release();
        }
    }

    m_samplers.clear();

    float* realloced = static_cast<float*>(std::realloc(m_udata, dataSize));
    if (realloced != nullptr) {
        m_udata = realloced;
    } else {
        LOG_ERROR("[Asset] Failed to realloc material {0} data array", this->getName());
        return;
    }

    std::memset(m_udata, 0, dataSize);

    for (size_t i = 0; i < m_type->getFields().NumSamplers; i++) {
        m_samplers.push_back(nullptr);
    }
}

bool MaterialAsset::isValid() const {
    if (m_type == nullptr || !m_type->isValid()) {
        return false;
    }

    for (asset::TextureAsset* ta : m_samplers) {
        if (ta == nullptr || !ta->isValid()) {
            return false;
        }
    }

    return true;
}

void MaterialAsset::load(const io::Loader* loader) {
    LOG_TRACE("[Asset] Loading {0}", this->getName());

    ::aderite::Engine::getLoaderPool()->enqueue(m_type, io::LoaderPool::Priority::HIGH);

    for (asset::TextureAsset* ta : m_samplers) {
        if (ta != nullptr && !ta->isValid()) {
            ::aderite::Engine::getLoaderPool()->enqueue(ta, io::LoaderPool::Priority::HIGH);
        }
    }

    LOG_INFO("[Asset] Loaded {0}", this->getName());
}

void MaterialAsset::unload() {
    LOG_TRACE("[Asset] Unloading {0}", this->getName());

    // TODO: Rework cause this should be reference counted

    LOG_INFO("[Asset] Unloaded {0}", this->getName());
}

bool MaterialAsset::needsLoading() const {
    if (m_type == nullptr || !m_type->isValid()) {
        // No type so no loading
        return false;
    }

    // Check if all samplers assigned
    for (asset::TextureAsset* ta : m_samplers) {
        if (ta == nullptr || !ta->isValid()) {
            return false;
        }
    }

    return !this->isValid();
}

reflection::Type MaterialAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::MATERIAL);
}

bool MaterialAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Material
    if (m_type == nullptr) {
        // Can't serialize anything
        return true;
    }

    emitter << YAML::Key << "MaterialType" << YAML::Value << m_type->getHandle();
    emitter << YAML::Key << "Properties" << YAML::BeginMap;
    emitter << YAML::Key << "Data" << YAML::Flow << YAML::BeginSeq;
    for (size_t i = 0; i < m_type->getFields().Size * 4; i++) {
        float val = m_udata[i];
        emitter << m_udata[i];
    }
    emitter << YAML::EndSeq;
    emitter << YAML::EndMap;

    emitter << YAML::Key << "Samplers" << YAML::Flow << YAML::BeginSeq;
    for (auto sampler : m_samplers) {
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

    io::SerializableHandle typeName = data["MaterialType"].as<io::SerializableHandle>();
    this->setMaterialType(static_cast<MaterialTypeAsset*>(::aderite::Engine::getAssetManager()->get(typeName)));

    const YAML::Node& d = data["Properties"]["Data"];
    if (d.size() != (m_type->getFields().Size * 4)) {
        LOG_ERROR("[Asset] Incorrect size for stored material data and type {0}", this->getName());
        return false;
    }

    for (size_t i = 0; i < d.size(); i++) {
        m_udata[i] = d[i].as<float>();
    }

    m_samplers.resize(data["Samplers"].size());

    size_t idx = 0;
    for (const YAML::Node& sampler : data["Samplers"]) {
        TextureAsset* texture = nullptr;

        if (!sampler.IsNull()) {
            this->setSampler(idx,
                             static_cast<TextureAsset*>(::aderite::Engine::getAssetManager()->get(sampler.as<io::SerializableHandle>())));
        }

        idx++;
    }

    return true;
}

void MaterialAsset::setSampler(size_t index, TextureAsset* ta) {
    ADERITE_DYNAMIC_ASSERT(this->getSamplerCount() > index, "Invalid sampler index");

    if (m_samplers[index] != nullptr) {
        m_samplers[index]->release();
    }

    m_samplers[index] = ta;
    m_samplers[index]->acquire();
}

size_t MaterialAsset::getSamplerCount() const {
    return m_samplers.size();
}

TextureAsset* MaterialAsset::getSampler(size_t index) const {
    ADERITE_DYNAMIC_ASSERT(this->getSamplerCount() > index, "Invalid sampler index");
    return m_samplers[index];
}

float* MaterialAsset::getPropertyData() const {
    return m_udata;
}

std::vector<std::pair<bgfx::UniformHandle, bgfx::TextureHandle>> MaterialAsset::getSamplerData() const {
    std::vector<std::pair<bgfx::UniformHandle, bgfx::TextureHandle>> result;

    for (size_t i = 0; i < m_type->getFields().NumSamplers; i++) {
        if (m_samplers[i] == nullptr) {
            result.push_back(std::make_pair(m_type->getSampler(i), bgfx::TextureHandle {bgfx::kInvalidHandle}));
        } else {
            result.push_back(std::make_pair(m_type->getSampler(i), m_samplers[i]->getTextureHandle()));
        }
    }

    return result;
}

} // namespace asset
} // namespace aderite
