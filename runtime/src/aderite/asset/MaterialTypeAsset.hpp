#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/asset/Asset.hpp"
#include "aderite/asset/property/Forward.hpp"
#include "aderite/asset/property/Property.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

/**
 * @brief Material type asset implementation
*/
class MaterialTypeAsset : public Asset {
public:
	using Properties = std::vector<prop::Property*>;

	/**
	 * @brief Editable fields of the asset, this information is stored inside the asset file
	*/
	struct fields {
		size_t ElementCount; // Computed from properties
		size_t v4Count;
		Properties Properties;
	};
public:
	~MaterialTypeAsset();

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
	 * @brief Recalculates property offsets
	*/
	void recalculate();

	/**
	 * @brief Returns the size in bytes
	*/
	size_t getSizeInBytes() const;

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

	bgfx::ProgramHandle getShaderHandle() const {
		return m_handle;
	}
	
	bgfx::UniformHandle getUniformHandle() const {
		return m_uniform;
	}
protected:
	MaterialTypeAsset(const std::string& name);
	MaterialTypeAsset(const std::string& name, const fields& info);

	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	friend class AssetManager;
private:
	// Shader
	bgfx::ProgramHandle m_handle = BGFX_INVALID_HANDLE;

	// Material properties
	bgfx::UniformHandle m_uniform = BGFX_INVALID_HANDLE;
	// TODO: Samplers

	fields m_info = {};

	std::vector<unsigned char> m_vertexSource;
	std::vector<unsigned char> m_fragmentSource;

	bool m_isBeingPrepared = false;
};

ADERITE_ASSET_NAMESPACE_END
