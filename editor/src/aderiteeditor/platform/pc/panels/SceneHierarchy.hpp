#pragma once

#include <vector>

#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Scene view is a component that displays all scene entities in a tree
 */
class SceneHierarchy : public UIComponent {
public:
    SceneHierarchy();
    virtual ~SceneHierarchy();

    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;

private:
    void contextMenu();
};

} // namespace editor
} // namespace aderite
