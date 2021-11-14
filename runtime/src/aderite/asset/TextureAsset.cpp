#include "TextureAsset.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace asset {

TextureAsset::~TextureAsset() {
    LOG_TRACE("[Asset] Destroying {0}", this->getHandle());

    if (bgfx::isValid(m_handle)) {
        this->unload();
    }
}

bool TextureAsset::isValid() const {
    return bgfx::isValid(m_handle);
}

void TextureAsset::load(const io::Loader* loader) {
    LOG_TRACE("[Asset] Loading {0}", this->getHandle());
    ADERITE_DYNAMIC_ASSERT(!bgfx::isValid(m_handle), "Tried to load already loaded texture");

    if (m_info.IsCubemap) {
        LOG_ERROR("Cubemap not implemented");
        return;
    } else {
        io::Loader::TextureLoadResult<unsigned char> result = loader->loadTexture(this->getHandle());
        if (!result.Error.empty()) {
            return;
        }

        m_handle = bgfx::createTexture2D(static_cast<uint16_t>(result.Width), static_cast<uint16_t>(result.Height), false, 1,
                                         static_cast<bgfx::TextureFormat::Enum>(result.Format),
                                         BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP,
                                         bgfx::copy(result.Data.get(), result.Width * result.Height * result.BPP));
    }

    LOG_INFO("[Asset] Loaded {0}", this->getHandle());
}

void TextureAsset::unload() {
    LOG_TRACE("[Asset] Unloading {0}", this->getHandle());

    if (bgfx::isValid(m_handle)) {
        bgfx::destroy(m_handle);
        m_handle = BGFX_INVALID_HANDLE;
    }

    LOG_INFO("[Asset] Unloaded {0}", this->getHandle());
}

bool TextureAsset::needsLoading() {
    return !this->isValid();
}

reflection::Type TextureAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::TEXTURE);
}

bool TextureAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "IsHDR" << YAML::Value << m_info.IsHDR;
    emitter << YAML::Key << "IsCubemap" << YAML::Value << m_info.IsCubemap;
    return true;
}

bool TextureAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_info.IsHDR = data["IsHDR"].as<bool>();
    m_info.IsCubemap = data["IsCubemap"].as<bool>();
    return true;
}

} // namespace asset
} // namespace aderite
