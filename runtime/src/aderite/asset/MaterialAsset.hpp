#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/IRenderable.hpp"
#include "aderite/asset/Asset.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

/**
 * @brief Material asset implementation
*/
class MaterialAsset : public Asset, public interfaces::IRenderable {
public:
	/**
	 * @brief Editable fields of the asset, this information is stored inside the asset file
	*/
	struct fields {
		//Asset* Shader;
	};
public:
	~MaterialAsset();

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

	// Inherited via IRenderable
	virtual void fillDrawCall(rendering::DrawCall* r) override;
protected:
	MaterialAsset(const std::string& name);
	MaterialAsset(const std::string& name, const fields& info);

	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	friend class AssetManager;
private:
	fields m_info = {};
	bool m_isBeingPrepared = false;
};

ADERITE_ASSET_NAMESPACE_END
