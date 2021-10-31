#pragma once

#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief NodeEditor of the editor, this is used by FPRP
 */
class NodeEditor : public editor::BaseComponent {
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

} // namespace editor_ui
} // namespace aderite
