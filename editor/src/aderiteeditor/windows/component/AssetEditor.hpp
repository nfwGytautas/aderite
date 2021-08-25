#pragma once

#include "aderite/asset/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief Asset editor is a component that allows to edit specific selected asset information
*/
class AssetEditor : public shared::BaseComponent {
public:
	AssetEditor();
	virtual ~AssetEditor();

	// Inherited via BaseComponent
	virtual void render() override;

	/**
	 * @brief Sets the current active asset
	*/
	void setActiveAsset(asset::Asset* asset);

private:
	/**
	 * @brief UI for shader asset
	*/
	void shaderRender();

	/**
	 * @brief UI for material asset
	*/
	void materialRender();

	/**
	 * @brief UI for mesh asset
	*/
	void meshRender();
private:
	asset::Asset* m_selectedAsset = nullptr;
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
