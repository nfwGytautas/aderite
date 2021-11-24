#pragma once

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include "aderite/scene/Forward.hpp"

#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief SceneView of the editor, this is the window where the game is actually rendered to
 */
class SceneView : public UIComponent {
public:
    /**
     * @brief Function invoked when the current active scene was changed
     * @param scene New active scene
     */
    void onSceneChanged(scene::Scene* scene);

    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;

private:
    bgfx::TextureHandle m_fbth = BGFX_INVALID_HANDLE;
    glm::uvec2 m_size = {};
};

} // namespace editor
} // namespace aderite
