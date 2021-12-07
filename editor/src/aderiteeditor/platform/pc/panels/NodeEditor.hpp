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
private:
    void renderMaterialEditorContextMenu();
};

} // namespace editor
} // namespace aderite
