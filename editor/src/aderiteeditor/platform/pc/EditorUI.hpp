#pragma once

#include <vector>

#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Class used to manage all editor UI components
 */
class EditorUI {
public:
    /**
     * @brief Initializes UI components and arranges them
     */
    bool setup();

    /**
     * @brief Renders enabled editor components
     */
    void render();

    /**
     * @brief Shutdown the editor UI
     */
    void shutdown();

private:
    /**
     * @brief Initializes ImGui and return true if initialized, false otherwise
     */
    bool initializeImGui();

    /**
     * @brief Shuts down ImGui context
     */
    void shutdownImGui();

    /**
     * @brief Configures dockspace
    */
    void configureDockspace();

    /**
     * @brief Creates UI components of the editor
     */
    void createComponents();

    /**
     * @brief Sets up ImGui style
     */
    void setupImGuiStyle();

private:
    // Components of the editor
    std::vector<UIComponent*> m_components;
};

} // namespace editor
} // namespace aderite
