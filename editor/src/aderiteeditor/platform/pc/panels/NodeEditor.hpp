#pragma once

#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief NodeEditor of the editor, this is used by FPRP
 */
class NodeEditor : public UIComponent {
public:
    enum class NodeEditorType {
        RENDER_PIPELINE,
        MATERIAL,
    };

public:
    NodeEditor();
    virtual ~NodeEditor();

    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;

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

} // namespace editor
} // namespace aderite
