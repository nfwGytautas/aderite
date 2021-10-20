#include "NodeEditor.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/shared/State.hpp"

// Nodes
// Material
#include "aderiteeditor/node/material/AddNode.hpp"
#include "aderiteeditor/node/material/MaterialInputNode.hpp"
#include "aderiteeditor/node/material/MaterialOutputNode.hpp"
#include "aderiteeditor/node/material/Sampler2DNode.hpp"

// Pipeline
#include "aderiteeditor/node/pipeline/CameraProviderNode.hpp"
#include "aderiteeditor/node/pipeline/ConcatObjectsNode.hpp"
#include "aderiteeditor/node/pipeline/EditorCameraNode.hpp"
#include "aderiteeditor/node/pipeline/EditorRenderNode.hpp"
#include "aderiteeditor/node/pipeline/EditorTargetNode.hpp"
#include "aderiteeditor/node/pipeline/EntitiesNode.hpp"
#include "aderiteeditor/node/pipeline/RenderNode.hpp"
#include "aderiteeditor/node/pipeline/RequireLockNode.hpp"
#include "aderiteeditor/node/pipeline/ScreenNode.hpp"
#include "aderiteeditor/node/pipeline/SelectObjectNode.hpp"
#include "aderiteeditor/node/pipeline/TargetProviderNode.hpp"

// Shared
#include "aderiteeditor/node/shared/Properties.hpp"
#include "aderiteeditor/node/shared/ConvertNode.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

constexpr int c_RootMaterialNodeId = 0;

NodeEditor::NodeEditor() {}

NodeEditor::~NodeEditor() {}

void NodeEditor::render() {
    if (ImGui::Begin("Node editor")) {
        if (m_graph) {
            bool pushed = false;
            if (aderite::Engine::getInputManager()->isKeyPressed(input::Key::LEFT_ALT)) {
                ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
                pushed = true;
            }
            ImNodes::BeginNodeEditor();

            {
                const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                    ImNodes::IsEditorHovered() &&
                    ImGui::IsMouseDown(ImGuiMouseButton_Right);

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
                if (!ImGui::IsAnyItemHovered() && open_popup)
                {
                    ImGui::OpenPopup("add node");
                }

                if (ImGui::BeginPopup("add node"))
                {
                    switch (m_type) {
                    case NodeEditorType::MATERIAL: {
                        renderMaterialEditorContextMenu();
                        break;
                    }
                    case NodeEditorType::RENDER_PIPELINE: {
                        renderRenderPipelineEditorContextMenu();
                        break;
                    }
                    }

                    ImGui::EndPopup();
                }
                ImGui::PopStyleVar();
            }

            m_graph->renderUI();

            ImNodes::MiniMap();
            ImNodes::EndNodeEditor();

            if (pushed) {
                ImNodes::PopAttributeFlag();
            }

            {
                int start;
                int end;
                if (ImNodes::IsLinkCreated(&start, &end)) {
                    m_graph->connect(start, end);
                }
            }

            {
                if (aderite::Engine::getInputManager()->isKeyPressed(input::Key::DEL)) {
                    std::vector<int> nodes;
                    nodes.resize(ImNodes::NumSelectedNodes());

                    if (nodes.size() > 0) {
                        ImNodes::GetSelectedNodes(nodes.data());

                        for (int node : nodes) {
                            m_graph->deleteNode(node);
                        }
                    }

                    ImNodes::ClearNodeSelection();
                }
            }

            {
                int link_id;
                if (ImNodes::IsLinkDestroyed(&link_id)) {
                    m_graph->disconnectLink(link_id);
                }
            }
        }
    }

    ImGui::End();
}

void NodeEditor::setGraph(node::Graph* graph, NodeEditorType type) {
    m_graph = graph;
    m_type = type;
}

void NodeEditor::renderMaterialEditorContextMenu() {
    const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

    if (ImGui::MenuItem("Add"))
    {
        node::Node* n = m_graph->addNode<node::AddNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::MenuItem("Sampler 2D"))
    {
        node::Node* n = m_graph->addNode<node::Sampler2DNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }
}

void NodeEditor::renderRenderPipelineEditorContextMenu() {
    const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

    if (ImGui::MenuItem("Entities"))
    {
        node::Node* n = m_graph->addNode<node::EntitiesNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::MenuItem("Camera provider"))
    {
        node::Node* n = m_graph->addNode<node::CameraProviderNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::MenuItem("Target provider"))
    {
        node::Node* n = m_graph->addNode<node::TargetProviderNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::BeginMenu("Array"))
    {
        if (ImGui::MenuItem("Concat"))
        {
            node::Node* n = m_graph->addNode<node::ConcatObjectsNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        if (ImGui::MenuItem("Select"))
        {
            node::Node* n = m_graph->addNode<node::SelectObjectNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Flow control"))
    {
        if (ImGui::MenuItem("Require lock"))
        {
            node::Node* n = m_graph->addNode<node::RequireLockNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Convert")) {
        node::Node* n = m_graph->addNode<node::ConvertNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::BeginMenu("Rendering"))
    {
        if (ImGui::MenuItem("Depth & Color"))
        {
            node::Node* n = m_graph->addNode<node::RenderNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::BeginMenu("Editor"))
    {
        if (ImGui::MenuItem("Render"))
        {
            node::Node* n = m_graph->addNode<node::EditorRenderNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        if (ImGui::MenuItem("Camera"))
        {
            node::Node* n = m_graph->addNode<node::EditorCameraNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        if (ImGui::MenuItem("Target"))
        {
            node::Node* n = m_graph->addNode<node::EditorTargetNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }


        ImGui::EndMenu();
    }
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
