#pragma once

#include "aderite/scene/Entity.hpp"

#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/windows/Forward.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief Scene view is a component that displays all scene entities in a tree
 */
class SceneHierarchy : public editor::BaseComponent {
public:
    SceneHierarchy();
    virtual ~SceneHierarchy();

    // Inherited via BaseComponent
    virtual void render() override;

private:
    TextInputModal* m_textModal = nullptr;
};

} // namespace editor_ui
} // namespace aderite