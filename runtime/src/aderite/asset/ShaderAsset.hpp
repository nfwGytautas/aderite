#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/IRenderable.hpp"
#include "aderite/asset/Asset.hpp"

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

	// Inherited via irenderable
	virtual void fillDrawCall(rendering::DrawCall* dc) override;
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

	bool m_isBeingPrepared = false;
};

ADERITE_ASSET_NAMESPACE_END
