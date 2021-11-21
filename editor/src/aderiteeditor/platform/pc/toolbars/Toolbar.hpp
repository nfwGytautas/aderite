#pragma once

#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Toolbar of the editor, this bar contains play, pause buttons etc.
 */
class Toolbar : public UIComponent {
public:
    Toolbar();
    virtual ~Toolbar();

    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;
};

} // namespace editor
} // namespace aderite
