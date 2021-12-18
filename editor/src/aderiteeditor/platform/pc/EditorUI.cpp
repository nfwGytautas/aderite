#include "EditorUI.hpp"

#include <bgfx/bgfx.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_internal.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/window/WindowManager.hpp"

#include "aderiteeditor/platform/pc/backend/bgfx/backend_bgfx.hpp"
#include "aderiteeditor/platform/pc/backend/node/imnodes.h"
#include "aderiteeditor/platform/pc/modals/StartupModal.hpp"
#include "aderiteeditor/platform/pc/panels/AssetBrowser.hpp"
#include "aderiteeditor/platform/pc/panels/Inspector.hpp"
#include "aderiteeditor/platform/pc/panels/NodeEditor.hpp"
#include "aderiteeditor/platform/pc/panels/SceneHierarchy.hpp"
#include "aderiteeditor/platform/pc/panels/SceneScriptMapping.hpp"
#include "aderiteeditor/platform/pc/panels/SceneView.hpp"
#include "aderiteeditor/platform/pc/toolbars/Menubar.hpp"
#include "aderiteeditor/platform/pc/toolbars/Toolbar.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor {

bool EditorUI::setup() {
    LOG_TRACE("[Editor] Initializing UI");

    // Initialize imgui
    if (!this->initializeImGui()) {
        return false;
    }

    // Create UI components
    this->createComponents();

    // Setup UI components
    LOG_TRACE("[Editor] Initializing UI components");
    for (UIComponent* component : m_components) {
        if (!component->init()) {
            return false;
        }
    }
    LOG_INFO("[Editor] UI components initialized");

    LOG_INFO("[Editor] UI initialized");
    return true;
}

void EditorUI::render() {
    // Start the Dear ImGui frame
    backend::ImGui_Implbgfx_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Note: Switch this to true to enable dockspace
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* SceneView = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(SceneView->Pos);
        ImGui::SetNextWindowSize(SceneView->Size);
        ImGui::SetNextWindowViewport(SceneView->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask
    // Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Dockspace Window", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen) {
        ImGui::PopStyleVar(2);
    }

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();

    ImGuiID dockspace_id = ImGui::GetID("Dockspace");
    this->configureDockspace();

    // Dockspace components start here

    // Render components
    for (UIComponent* component : m_components) {
        component->render();
    }

    // Dockspace components end here

    // Modal
    if (m_modalStack.size() > 0) {
        IModal* modal = m_modalStack.top();
        modal->show();
        if (modal->stillValid()) {
            modal->render();
        } else {
            // Delete the top one and pop it
            delete modal;
            modal = nullptr;

            m_modalStack.pop();
        }
    }

    // Rendering ImGui
    ImGui::End();
    ImGui::Render();
    backend::ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
}

void EditorUI::shutdown() {
    LOG_TRACE("[Editor] Shutting down UI");

    // Shutdown UI components
    LOG_TRACE("[Editor] Shutting down UI components");
    for (UIComponent* component : m_components) {
        component->shutdown();
    }
    LOG_INFO("[Editor] UI components shutdown");

    // Shutdown imgui
    this->shutdownImGui();

    LOG_INFO("[Editor] UI shutdown");
}

void EditorUI::pushModal(IModal* modal) {
    m_modalStack.push(modal);
}

bool EditorUI::initializeImGui() {
    // Initialize ImGui
    LOG_TRACE("[Editor] Initializing ImGui");

    // Create context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext();

    // Style
    this->setupImGuiStyle();

    // Setup Platform/Renderer backends
    GLFWwindow* handle = static_cast<GLFWwindow*>(::aderite::Engine::get()->getWindowManager()->getImplementationHandle());

    switch (bgfx::getRendererType()) {
    case bgfx::RendererType::OpenGL: {
        ImGui_ImplGlfw_InitForOpenGL(handle, true);
        break;
    }
    case bgfx::RendererType::Vulkan: {
        ImGui_ImplGlfw_InitForVulkan(handle, true);
        break;
    }
    case bgfx::RendererType::Direct3D11:
    case bgfx::RendererType::Direct3D12: {
        ImGui_ImplGlfw_InitForOther(handle, true);
        break;
    }
    default:
        LOG_ERROR("[Editor] Non supported bgfx Renderer type {0} for editor", bgfx::getRendererType());
        return false;
    }
    backend::ImGui_Implbgfx_Init(255);

    LOG_INFO("[Editor] ImGui initialized");
    return true;
}

void EditorUI::shutdownImGui() {
    // Shutdown ImGui
    backend::ImGui_Implbgfx_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImNodes::DestroyContext();
    ImGui::DestroyContext();
}

void EditorUI::configureDockspace() {
    // TODO: Implement default dockspace layout
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    ImGuiID dockspaceID = ImGui::GetID("Dockspace");
    if (!ImGui::DockBuilderGetNode(dockspaceID)) {
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);

        ImGuiID dockMainId = dockspaceID;

        // -------- MENU BAR ------------------------
        // -------- TOOL BAR ------------------------
        // SCENE     |    SCENE VIEW    | INSPECTOR
        // HIERARCHY |                  |
        //           |                  |
        // ------ ASSET BROWSER ---------------------

        // Toolbar, immutable
        ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Up, 0.05f, nullptr, &dockMainId);

        // Asset browser on complete bottom
        ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Down, 0.3f, nullptr, &dockMainId);

        // Scene view
        ImGuiID dockMiddleLeft = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Left, 0.2f, nullptr, &dockMainId);

        // Inspector
        ImGuiID dockMiddleRight = ImGui::DockBuilderSplitNode(dockMainId, ImGuiDir_Right, 0.3f, nullptr, &dockMainId);

        // Setup
        ImGui::DockBuilderDockWindow("Toolbar", dockTop);
        ImGui::DockBuilderDockWindow("Scene hierarchy", dockMiddleLeft);
        ImGui::DockBuilderDockWindow("Asset browser", dockBottom);
        ImGui::DockBuilderDockWindow("Inspector", dockMiddleRight);
        ImGui::DockBuilderDockWindow("Scene view", dockMainId);
        ImGui::DockBuilderDockWindow("Node editor", dockMainId);
        ImGui::DockBuilderDockWindow("Script mapping", dockMainId);

        // Make toolbar immutable
        ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockTop);
        node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoDockingSplitMe |
                            ImGuiDockNodeFlags_NoDockingOverMe;

        ImGui::DockBuilderFinish(dockMainId);
    }

    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
}

void EditorUI::createComponents() {
    LOG_TRACE("[Editor] Creating UI components");

    // Toolbars
    m_components.push_back(new editor::Menubar());
    m_components.push_back(new editor::Toolbar());

    // Panels
    m_components.push_back(new editor::Inspector());
    m_components.push_back(new editor::SceneView());
    m_components.push_back(new editor::SceneHierarchy());
    m_components.push_back(new editor::AssetBrowser());
    m_components.push_back(new editor::NodeEditor());
    m_components.push_back(new editor::SceneScriptMapping());

    // Other
    m_components.push_back(new editor::StartupModal());

    LOG_INFO("[Editor] {0} UI components created", m_components.size());
}

void EditorUI::setupImGuiStyle() {
    // Aderite styling, inspired by visual studio and alike
    constexpr float mainBgHue = 45.0f / 255.0f;
    constexpr float secondaryBgHue = 25.0f / 255.0f;
    constexpr float textColorHue = 255.0f / 255.0f;
    constexpr float hoverHue = 72.0f / 255.0f;
    constexpr float titleHue = 96.0f / 255.0f;
    constexpr float activeTabHue = 72.0f / 255.0f;
    constexpr float checkmarkHue = 154.0f / 255.0f;

    constexpr float headerBgHue = 62.0f / 255.0f;

    constexpr float scrollbarBgHue = 62.0f / 255.0f;
    constexpr float scrollbarHue = 104.0f / 255.0f;
    constexpr float scrollbarActiveHue = 158.0f / 255.0f;

    constexpr float frameBgHue = 100.0f / 255.0f;
    constexpr float frameBgHoverHue = 97.0f / 255.0f;

    constexpr float brighter = 1.2f;

    LOG_TRACE("[Editor] Setting up style");
    // Get IO object
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Light as baseline
    ImGui::StyleColorsLight();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
    }

    // Font
    io.Fonts->AddFontFromFileTTF("res/fonts/Consolas.ttf", 14);

    // Apply to style object
    style.FrameBorderSize = 1.0f;

    // Windows
    style.Colors[ImGuiCol_WindowBg] = ImVec4(mainBgHue, mainBgHue, mainBgHue, 1.0);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(secondaryBgHue, secondaryBgHue, secondaryBgHue, 1.0);

    style.Colors[ImGuiCol_TitleBg] = ImVec4(titleHue, titleHue, titleHue, 1.0);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(titleHue, titleHue, titleHue, 1.0);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(mainBgHue, mainBgHue, mainBgHue, 1.0);

    // Tabs
    style.TabRounding = 0.0f;
    style.Colors[ImGuiCol_Tab] = ImVec4(mainBgHue, mainBgHue, mainBgHue, 1.0);
    style.Colors[ImGuiCol_TabActive] = ImVec4(activeTabHue, activeTabHue, activeTabHue, 1.0);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(mainBgHue, mainBgHue, mainBgHue, 1.0);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(activeTabHue, activeTabHue, activeTabHue, 1.0);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(hoverHue, hoverHue, hoverHue, 1.0);

    // Trees
    style.Colors[ImGuiCol_Header] = ImVec4(headerBgHue, headerBgHue, headerBgHue, 1.0);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(hoverHue, hoverHue, hoverHue, 1.0);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(hoverHue, hoverHue, hoverHue, 1.0);

    // Text
    style.Colors[ImGuiCol_Text] = ImVec4(textColorHue, textColorHue, textColorHue, 1.0);

    // Button
    style.Colors[ImGuiCol_Button] = ImVec4(mainBgHue, mainBgHue, mainBgHue, 1.0);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(hoverHue, hoverHue, hoverHue, 1.0);

    // Checkbox, radiobutton, etc.
    style.Colors[ImGuiCol_FrameBg] = ImVec4(frameBgHue, frameBgHue, frameBgHue, 90.0f / 255.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(frameBgHoverHue, frameBgHoverHue, frameBgHoverHue, 1.0);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(checkmarkHue, checkmarkHue, checkmarkHue, 1.0);

    // Scrollbar
    style.ScrollbarRounding = 0.0f;
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(scrollbarBgHue, scrollbarBgHue, scrollbarBgHue, 1.0);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(scrollbarHue, scrollbarHue, scrollbarHue, 1.0);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(scrollbarActiveHue, scrollbarActiveHue, scrollbarActiveHue, 1.0);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(scrollbarActiveHue, scrollbarActiveHue, scrollbarActiveHue, 1.0);
}

} // namespace editor
} // namespace aderite
