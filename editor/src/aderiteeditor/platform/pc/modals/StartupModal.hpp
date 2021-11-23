#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief This is the modal window showed on aderite editor startup, this is where a load/new project buttons are
 * placed in
 */
class StartupModal : public UIComponent {
public:
    /**
     * @brief Shows the modal window
     */
    void show();

    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;

    /**
     * @brief Returns true if the modal is currently visible, false otherwise
     */
    bool isOpen() const;

private:
    void renderMain();
    void renderNewProject();

private:
    bool m_visible = false;
    bool m_show = false;
    bool m_newProject = false;

    std::string m_newProjectName = "";
    std::string m_newProjectRoot = "";
    std::string m_newProjectRootPreview = "";
    int m_template = 0;

};

} // namespace editor_ui
} // namespace aderite
