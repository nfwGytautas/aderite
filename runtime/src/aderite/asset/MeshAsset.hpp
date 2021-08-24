#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/macros.hpp"
#include "aderite/interfaces/IRenderable.hpp"
#include "aderite/asset/asset.hpp"
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
		bool IsStatic = false;
		bool HasPosition = true;
		bool HasIndices = true;
	};
public:
	~MeshAsset();

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

	bool m_being_prepared = false;
};

ADERITE_ASSET_NAMESPACE_END
