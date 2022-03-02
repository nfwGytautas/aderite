#pragma once

#include <vector>
#include <stack>

#include "aderiteeditor/platform/pc/UIComponent.hpp"
#include "aderiteeditor/platform/pc/modals/IModal.hpp"

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

    /**
     * @brief Push modal to the UI
     * @param modal Modal to push
     */
    void pushModal(IModal* modal);

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

    // No modal by default
    std::stack<IModal*> m_modalStack;
};

} // namespace editor
} // namespace aderite
