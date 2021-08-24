#pragma once

#include <bgfx/bgfx.h>
#include "aderite/interfaces/IRenderable.hpp"
#include "aderite/asset/Asset.hpp"
#include "aderite/utility/macros.hpp"

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
		Asset* Shader;
	};
public:
	~MaterialAsset();

	virtual AssetType type() const override;
	virtual bool isInGroup(AssetGroup group) const override;

	virtual void prepare_load() override;
	virtual bool ready_to_load() override;
	virtual void load() override;
	virtual void unload() override;
	virtual bool is_preparing() override;
	virtual bool is_loaded() override;

	/**
	 * @brief Returns the info of shader fields
	*/
	fields get_fields() const {
		return m_info;
	}

	/**
	 * @brief Returns mutable field structure
	*/
	fields& get_fields_mutable() {
		return m_info;
	}

	// Inherited via IRenderable
	virtual void fill_draw_call(rendering::DrawCall* r) override;
protected:
	MaterialAsset(const std::string& name);
	MaterialAsset(const std::string& name, const fields& info);

	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	friend class AssetManager;
private:
	fields m_info = {};
	bool m_being_prepared = false;
};

ADERITE_ASSET_NAMESPACE_END
