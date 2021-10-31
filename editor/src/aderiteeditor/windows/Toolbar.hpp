#pragma once

#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/windows/Forward.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief Toolbar of the editor, this bar contains play, pause buttons etc.
 */
class Toolbar : public editor::BaseComponent {
public:
    Toolbar();
    virtual ~Toolbar();

    // Inherited via BaseComponent
    virtual void render() override;
};

} // namespace editor_ui
} // namespace aderite
