#pragma once

#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/node/Graph.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief NodeEditor of the editor, this is used by FPRP
*/
class NodeEditor : public shared::BaseComponent {
public:
	enum class NodeEditorType {
		RENDER_PIPELINE,
		MATERIAL,
	};

public:
	NodeEditor();
	virtual ~NodeEditor();

	// Inherited via BaseComponent
	virtual void render() override;

	/**
	 * @brief Sets the current active asset
	*/
	void setGraph(node::Graph* graph, NodeEditorType type);
private:
	void renderMaterialEditorContextMenu();
	void renderRenderPipelineEditorContextMenu();
private:
	node::Graph* m_graph = nullptr;
	NodeEditorType m_type = NodeEditorType::RENDER_PIPELINE;
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
