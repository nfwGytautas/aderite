#pragma once

#include "aderite/scene/Entity.hpp"

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
    /**
     * @brief Render context menu
     */
    void renderContextMenu();

    /**
     * @brief Renders visuals tree node of the scene
    */
    void renderVisuals();

    /**
     * @brief Renders scenery tree node of the scene
    */
    void renderScenery();

    /**
     * @brief Renders audio tree node of the scene
     */
    void renderAudio();

    /**
     * @brief Renders scripts tree node of the scene
     */
    void renderScripts();

    /**
     * @brief Renders settings nodes of the scene
     */
    void renderSettings();
};

} // namespace editor
} // namespace aderite
