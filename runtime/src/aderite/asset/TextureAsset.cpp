#include "TextureAsset.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/TextureSource.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

TextureAsset::~TextureAsset() {
	if (bgfx::isValid(m_handle)) {
		LOG_WARN("Deleting a loaded texture asset {0}", getName());
	}
}

bgfx::TextureHandle TextureAsset::getHandle() {
	return m_handle;
}

AssetType TextureAsset::type() const {
	return AssetType::TEXTURE;
}

bool TextureAsset::isInGroup(AssetGroup group) const {
	switch (group) {
	case AssetGroup::DEPENDS_ON_RAW:
	{
		return true;
	}
	default:
		return false;
	}
}

void TextureAsset::prepareLoad() {
	// Load sources
	::aderite::Engine::getAssetManager()->loadTextureSource(m_info.SourceFile, [&](asset::TextureSource* source) {
		// Start loading the data
		if (m_info.IsHDR) {
			source->loadHdr();
		}
		else {
			source->load();
		}

		if (m_source != nullptr) {
			delete m_source;
		}

		m_source = source;
	});
	m_isBeingPrepared = true;
}

bool TextureAsset::isReadyToLoad() {
	return m_source != nullptr && (m_source->error() == asset::TextureSource::LoadError::NONE);
}

void TextureAsset::load() {
	if (isLoaded()) {
		LOG_WARN("Loading an already loaded asset {0}, is this intended?", p_name);
		unload();
	}

	if (m_info.IsCubemap) {
		LOG_ERROR("Cubemap not implemented");
		return;
	}
	else {
		m_handle = bgfx::createTexture2D(
			static_cast<uint16_t>(m_source->getWidth()),
			static_cast<uint16_t>(m_source->getHeight()),
			false,
			1,
			m_source->getFormat(),
			BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP,
			bgfx::makeRef(m_source->getData(), m_source->getSize()));
	}

	m_isBeingPrepared = false;
}

void TextureAsset::unload() {
	if (bgfx::isValid(m_handle)) {
		bgfx::destroy(m_handle);
		m_handle = BGFX_INVALID_HANDLE;
	}

	if (m_source != nullptr) {
		delete m_source;
		m_source = nullptr;
	}
}

bool TextureAsset::isPreparing() {
	return m_isBeingPrepared;
}

bool TextureAsset::isLoaded() {
	return bgfx::isValid(m_handle);
}

size_t TextureAsset::hash() const {
	size_t thisHash = std::hash<std::string>{}(p_name);
	return thisHash;
}

TextureAsset::TextureAsset(const std::string& name)
	: Asset(name + ".texture")
{}

TextureAsset::TextureAsset(const std::string& name, const fields& info)
	: Asset(name + ".texture"), m_info(info)
{}

bool TextureAsset::serialize(YAML::Emitter& out) {
	out << YAML::Key << "Source" << YAML::Value << m_info.SourceFile;
	out << YAML::Key << "IsHDR" << YAML::Value << m_info.IsHDR;
	out << YAML::Key << "IsCubemap" << YAML::Value << m_info.IsCubemap;

	return true;
}

bool TextureAsset::deserialize(YAML::Node& data) {
	if (data["Source"]) {
		m_info.SourceFile = data["Source"].as<std::string>();
	}

	m_info.IsHDR = data["IsHDR"].as<bool>();
	m_info.IsCubemap = data["IsCubemap"].as<bool>();

	return true;
}

ADERITE_ASSET_NAMESPACE_END
