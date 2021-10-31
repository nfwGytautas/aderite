#pragma once

#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/windows/Forward.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief Menubar of the editor, the bar that goes at the top of the window
 */
class Menubar : public editor::BaseComponent {
public:
    Menubar();
    virtual ~Menubar();

    // Inherited via BaseComponent
    virtual void render() override;

private:
    TextInputModal* m_textModal = nullptr;
};

} // namespace editor_ui
} // namespace aderite
