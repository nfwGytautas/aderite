#pragma once

#include "aderite/asset/Asset.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/windows/component/Forward.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief NodeEditor of the editor, this is used by FPRP
*/
class NodeEditor : public shared::BaseComponent {
public:
	NodeEditor();
	virtual ~NodeEditor();

	// Inherited via BaseComponent
	virtual void init() override;
	virtual void render() override;
	virtual void shutdown() override;

	/**
	 * @brief Sets the current active asset
	*/
	void setActiveAsset(asset::Asset* asset);
private:
	node::Graph* m_currentState = nullptr;
	asset::Asset* m_selectedAsset = nullptr;
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
