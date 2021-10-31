#pragma once

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include "aderite/scene/Forward.hpp"

#include "aderiteeditor/shared/BaseComponent.hpp"
#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief SceneView of the editor, this is the window where the game is actually rendered to
 */
class SceneView : public editor::BaseComponent {
public:
    // Inherited via BaseComponent
    virtual void init() override;
    virtual void shutdown() override;
    virtual void render() override;

    /**
     * @brief Function invoked when the current active scene was changed
     * @param scene New active scene
     */
    void onSceneChanged(scene::Scene* scene);

private:
    bgfx::TextureHandle m_fbth = BGFX_INVALID_HANDLE;
    glm::uvec2 m_size = {};
};

} // namespace editor_ui
} // namespace aderite
