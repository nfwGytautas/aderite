#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/macros.hpp"
#include "aderite/interfaces/IRenderable.hpp"
#include "aderite/asset/asset.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

/**
 * @brief Shader asset implementation
*/
class ShaderAsset : public Asset, public interfaces::IRenderable {
public:
	/**
	 * @brief Editable fields of the asset, this information is stored inside the asset file
	*/
	struct fields {
		std::string VertexPath = "";
		std::string FragmentPath = "";
	};
public:
	~ShaderAsset();

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

	// Inherited via irenderable
	virtual void fill_draw_call(rendering::DrawCall* dc) override;
protected:
	ShaderAsset(const std::string& name);
	ShaderAsset(const std::string& name, const fields& info);

	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	friend class AssetManager;
private:
	// Handles
	bgfx::ProgramHandle m_handle = BGFX_INVALID_HANDLE;

	fields m_info = {};

	std::vector<unsigned char> m_vertexSource;
	std::vector<unsigned char> m_fragmentSource;

	bool m_being_prepared = false;
};

ADERITE_ASSET_NAMESPACE_END
