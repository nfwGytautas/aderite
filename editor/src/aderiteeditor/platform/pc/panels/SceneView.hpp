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
    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;

private:
    glm::uvec2 m_size = {};
};

} // namespace editor
} // namespace aderite
