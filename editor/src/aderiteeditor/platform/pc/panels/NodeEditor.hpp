#pragma once

#include "aderiteeditor/node/new/GraphX.hpp"
#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief NodeEditor of the editor, this is used by FPRP
 */
class NodeEditor : public UIComponent {
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
    void setGraph(node::Graph* graph);

private:
    void renderMaterialEditorContextMenu();

private:
    node::Graph* m_graph = nullptr;
};

} // namespace editor
} // namespace aderite
