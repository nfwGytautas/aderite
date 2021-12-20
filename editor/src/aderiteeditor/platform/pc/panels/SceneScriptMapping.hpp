#pragma once

#include "aderite/scripting/Forward.hpp"

#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Scene script mapping component used to display the mapping of engine callbacks for the current scene
 */
class SceneScriptMapping : public UIComponent {
public:
    SceneScriptMapping();
    virtual ~SceneScriptMapping();

    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;
};

} // namespace editor
} // namespace aderite
