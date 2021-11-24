#pragma once

#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Menubar of the editor, the bar that goes at the top of the window
 */
class Menubar : public UIComponent {
public:
    Menubar();
    virtual ~Menubar();

    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;
};

} // namespace editor
} // namespace aderite
