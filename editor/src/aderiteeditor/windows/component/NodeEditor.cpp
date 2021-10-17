#include "NodeEditor.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/compiler/Compiler.hpp"
#include "aderiteeditor/asset/RenderingPipeline.hpp"
#include "aderiteeditor/runtime/Resolver.hpp"
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


#include <fstream>
#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

constexpr int c_RootMaterialNodeId = 0;

NodeEditor::NodeEditor() {}

NodeEditor::~NodeEditor() {}

void NodeEditor::init() {
    //// TEMPORARY RENDER PIPELINE GRAPH
    //m_currentState = new node::Graph();
    //
    //node::Node* output = m_currentState->addNode<node::ScreenNode>();
    //m_currentState->setLastNode(output);
    //ImNodes::SetNodeEditorSpacePos(output->getId(), ImVec2(50, 50));
}

void NodeEditor::render() {
    if (ImGui::Begin("Node editor")) {
        if (ImGui::Button("Compile")) {
            /*if (m_selectedAsset && m_selectedAsset->type() == asset::AssetType::MATERIAL_TYPE) {
                compiler::Compiler::compileGraph(m_currentState);
            }
            else {
                
            }*/
            compiler::Compiler::compilePipeline(m_pipeline->getGraph());
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            ::aderite::Engine::getSerializer()->save(m_pipeline);
        }
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            // TODO: Fix this
            m_pipeline = static_cast<asset::RenderingPipeline*>(::aderite::Engine::getSerializer()->getOrRead(0));
            ::aderite::Engine::getSerializer()->reread(m_pipeline);
        }

        if (m_pipeline) {
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
                    /*if (m_selectedAsset && m_selectedAsset->type() == asset::AssetType::MATERIAL_TYPE) {
                        renderMaterialEditorContextMenu();
                    }
                    else {
                        renderRenderPipelineEditorContextMenu();
                    }*/

                    renderRenderPipelineEditorContextMenu();

                    ImGui::EndPopup();
                }
                ImGui::PopStyleVar();
            }


            m_pipeline->getGraph()->renderUI();

            ImNodes::MiniMap();
            ImNodes::EndNodeEditor();

            if (pushed) {
                ImNodes::PopAttributeFlag();
            }

            {
                int start;
                int end;
                if (ImNodes::IsLinkCreated(&start, &end)) {
                    m_pipeline->getGraph()->connect(start, end);
                }
            }

            {
                if (aderite::Engine::getInputManager()->isKeyPressed(input::Key::DEL)) {
                    std::vector<int> nodes;
                    nodes.resize(ImNodes::NumSelectedNodes());

                    if (nodes.size() > 0) {
                        ImNodes::GetSelectedNodes(nodes.data());

                        for (int node : nodes) {
                            m_pipeline->getGraph()->deleteNode(node);
                        }
                    }

                    ImNodes::ClearNodeSelection();
                }
            }

            {
                int link_id;
                if (ImNodes::IsLinkDestroyed(&link_id)) {
                    m_pipeline->getGraph()->disconnectLink(link_id);
                }
            }
        }
    }

    ImGui::End();
}

void NodeEditor::shutdown() {

}

void NodeEditor::setActiveAsset(asset::Asset* asset) {
    m_pipeline = new asset::RenderingPipeline();
    ::aderite::Engine::getSerializer()->add(m_pipeline);
    //m_selectedAsset = asset;
    //
    //if (m_selectedAsset->type() == asset::AssetType::MATERIAL_TYPE) {
    //    if (!m_currentState) {
    //        delete m_currentState;
    //    }
    //
    //    // TODO: Load already created graph for asset
    //
    //    m_currentState = new node::Graph();
    //
    //    node::Node* output = m_currentState->addNode<node::MaterialOutputNode>();
    //    node::Node* input = m_currentState->addNode<node::MaterialInputNode>();
    //    m_currentState->setLastNode(output);
    //    ImNodes::SetNodeEditorSpacePos(input->getId(), ImVec2(50, 50));
    //    ImNodes::SetNodeEditorSpacePos(output->getId(), ImVec2(350, 50));
    //}
}

void NodeEditor::renderMaterialEditorContextMenu() {
    const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

    if (ImGui::MenuItem("Add"))
    {
        node::Node* n = m_pipeline->getGraph()->addNode<node::AddNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::MenuItem("Sampler 2D"))
    {
        node::Node* n = m_pipeline->getGraph()->addNode<node::Sampler2DNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }
}

void NodeEditor::renderRenderPipelineEditorContextMenu() {
    const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

    if (ImGui::MenuItem("Entities"))
    {
        node::Node* n = m_pipeline->getGraph()->addNode<node::EntitiesNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::MenuItem("Camera provider"))
    {
        node::Node* n = m_pipeline->getGraph()->addNode<node::CameraProviderNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::MenuItem("Target provider"))
    {
        node::Node* n = m_pipeline->getGraph()->addNode<node::TargetProviderNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::BeginMenu("Array"))
    {
        if (ImGui::MenuItem("Concat"))
        {
            node::Node* n = m_pipeline->getGraph()->addNode<node::ConcatObjectsNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        if (ImGui::MenuItem("Select"))
        {
            node::Node* n = m_pipeline->getGraph()->addNode<node::SelectObjectNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Flow control"))
    {
        if (ImGui::MenuItem("Require lock"))
        {
            node::Node* n = m_pipeline->getGraph()->addNode<node::RequireLockNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Convert")) {
        node::Node* n = m_pipeline->getGraph()->addNode<node::ConvertNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::BeginMenu("Rendering"))
    {
        if (ImGui::MenuItem("Depth & Color"))
        {
            node::Node* n = m_pipeline->getGraph()->addNode<node::RenderNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::BeginMenu("Editor"))
    {
        if (ImGui::MenuItem("Render"))
        {
            node::Node* n = m_pipeline->getGraph()->addNode<node::EditorRenderNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        if (ImGui::MenuItem("Camera"))
        {
            node::Node* n = m_pipeline->getGraph()->addNode<node::EditorCameraNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        if (ImGui::MenuItem("Target"))
        {
            node::Node* n = m_pipeline->getGraph()->addNode<node::EditorTargetNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }


        ImGui::EndMenu();
    }
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
