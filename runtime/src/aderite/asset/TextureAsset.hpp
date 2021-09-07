#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/IRenderable.hpp"
#include "aderite/asset/Asset.hpp"
#include "aderite/asset/Forward.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

/**
 * @brief Texture asset implementation
*/
class TextureAsset : public Asset {
public:
	/**
	 * @brief Editable fields of the asset, this information is stored inside the asset file
	*/
	struct fields {
		std::string SourceFile = "";

		// TODO: Encapsulate
		bool IsHDR = false;
		bool IsCubemap = false;
	};
public:
	~TextureAsset();

	/**
	 * @brief Returns texture handle of this asset
	*/
	bgfx::TextureHandle getHandle();

	virtual AssetType type() const override;
	virtual bool isInGroup(AssetGroup group) const override;

	virtual void prepareLoad() override;
	virtual bool isReadyToLoad() override;
	virtual void load() override;
	virtual void unload() override;
	virtual bool isPreparing() override;
	virtual bool isLoaded() override;
	virtual size_t hash() const override;

	/**
	 * @brief Returns the info of shader fields
	*/
	fields getFields() const {
		return m_info;
	}

	/**
	 * @brief Returns mutable field structure
	*/
	fields& getFieldsMutable() {
		return m_info;
	}
protected:
	TextureAsset(const std::string& name);
	TextureAsset(const std::string& name, const fields& info);

	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	friend class AssetManager;
private:
	bgfx::TextureHandle m_handle = BGFX_INVALID_HANDLE;

	fields m_info = {};
	TextureSource* m_source = nullptr;

	bool m_isBeingPrepared = false;
};

ADERITE_ASSET_NAMESPACE_END
