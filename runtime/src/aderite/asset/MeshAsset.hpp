#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/interfaces/IRenderable.hpp"
#include "aderite/asset/Asset.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderite/rendering/Forward.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

/**
 * @brief Mesh asset implementation, it just wraps around base mesh object, to provide interface with asset manager
*/
class MeshAsset : public Asset, public interfaces::IRenderable {
public:
	/**
	 * @brief Editable fields of the asset, this information is stored inside the asset file
	*/
	struct fields {
		std::string SourceFile = "";
		bool IsStatic = true;
		bool HasPosition = true;
		bool HasIndices = true;
	};
public:
	~MeshAsset();

	virtual AssetType type() const override;
	virtual bool isInGroup(AssetGroup group) const override;

	virtual void prepareLoad() override;
	virtual bool isReadyToLoad() override;
	virtual void load() override;
	virtual void unload() override;
	virtual bool isPreparing() override;
	virtual bool isLoaded() override;

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
	MeshAsset(const std::string& name);
	MeshAsset(const std::string& name, const fields& info);

	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

	friend class AssetManager;
private:
	// BGFX resource handles
	bgfx::VertexBufferHandle m_vbh = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle m_ibh = BGFX_INVALID_HANDLE;

	fields m_info = {};
	MeshSource* m_source = nullptr;

	bool m_isBeingPrepared = false;
};

ADERITE_ASSET_NAMESPACE_END
