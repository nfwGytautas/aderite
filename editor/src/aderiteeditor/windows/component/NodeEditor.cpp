#include "NodeEditor.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/compiler/Compiler.hpp"

// Nodes

#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/node/ConvertNode.hpp"

// Material
#include "aderiteeditor/node/MaterialInputNode.hpp"
#include "aderiteeditor/node/MaterialOutputNode.hpp"
#include "aderiteeditor/node/Sampler2DNode.hpp"
#include "aderiteeditor/node/AddNode.hpp"

// Render pipeline
#include "aderiteeditor/node/pipeline/ScreenNode.hpp"
#include "aderiteeditor/node/pipeline/EntitiesNode.hpp"
#include "aderiteeditor/node/pipeline/CameraProviderNode.hpp"
#include "aderiteeditor/node/pipeline/TargetProviderNode.hpp"
#include "aderiteeditor/node/pipeline/RenderNode.hpp"
#include "aderiteeditor/node/pipeline/EditorRenderNode.hpp"
#include "aderiteeditor/node/pipeline/EditorTargetNode.hpp"
#include "aderiteeditor/node/pipeline/EditorCameraNode.hpp"

#include <fstream>
#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

constexpr int c_RootMaterialNodeId = 0;

NodeEditor::NodeEditor() {}

NodeEditor::~NodeEditor() {}

void NodeEditor::init() {
    // TEMPORARY RENDER PIPELINE GRAPH
    m_currentState = new node::Graph();

    node::Node* output = m_currentState->addNode<node::ScreenNode>();
    m_currentState->setLastNode(output);
    ImNodes::SetNodeEditorSpacePos(output->getId(), ImVec2(50, 50));
}

void NodeEditor::render() {
    if (ImGui::Begin("Node editor")) {
        if (ImGui::Button("Compile")) {
            if (m_selectedAsset && m_selectedAsset->type() == asset::AssetType::MATERIAL_TYPE) {
                compiler::Compiler::compileGraph(m_currentState);
            }
            else {
                compiler::Compiler::compilePipeline(m_currentState);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            YAML::Emitter out;

            out << YAML::BeginMap;

            // Common
            out << YAML::Key << "Version" << YAML::Value << "test";
            //out << YAML::Key << "Name" << YAML::Value << asset->getName();
            out << YAML::Key << "Type" << YAML::Value << "test";

            m_currentState->serialize(out);

            out << YAML::EndMap;

            std::ofstream fout((aderite::Engine::getAssetManager()->getResDir() / "graph.yaml"));
            fout << out.c_str();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            // Open YAML reader
            if (!std::filesystem::exists((aderite::Engine::getAssetManager()->getResDir() / "graph.yaml"))) {
                return;
            }
            delete m_currentState;
            m_currentState = new node::Graph();

            YAML::Node data = YAML::LoadFile((aderite::Engine::getAssetManager()->getResDir() / "graph.yaml").string());
            m_currentState->deserialize(data);
        }

        if (m_currentState) {
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
                    if (m_selectedAsset && m_selectedAsset->type() == asset::AssetType::MATERIAL_TYPE) {
                        renderMaterialEditorContextMenu();
                    }
                    else {
                        renderRenderPipelineEditorContextMenu();
                    }

                    ImGui::EndPopup();
                }
                ImGui::PopStyleVar();
            }


            m_currentState->renderUI();

            ImNodes::MiniMap();
            ImNodes::EndNodeEditor();

            if (pushed) {
                ImNodes::PopAttributeFlag();
            }

            {
                int start;
                int end;
                if (ImNodes::IsLinkCreated(&start, &end)) {
                    m_currentState->connect(start, end);
                }
            }

            {
                if (aderite::Engine::getInputManager()->isKeyPressed(input::Key::DEL)) {
                    std::vector<int> nodes;
                    nodes.resize(ImNodes::NumSelectedNodes());
                    ImNodes::GetSelectedNodes(nodes.data());

                    for (int node : nodes) {
                        m_currentState->deleteNode(node);
                    }
                }
            }

            {
                int link_id;
                if (ImNodes::IsLinkDestroyed(&link_id)) {
                    m_currentState->disconnectLink(link_id);
                }
            }
        }
    }

    ImGui::End();
}

void NodeEditor::shutdown() {

}

void NodeEditor::setActiveAsset(asset::Asset* asset) {
    m_selectedAsset = asset;

    if (m_selectedAsset->type() == asset::AssetType::MATERIAL_TYPE) {
        if (!m_currentState) {
            delete m_currentState;
        }

        // TODO: Load already created graph for asset

        m_currentState = new node::Graph();

        node::Node* output = m_currentState->addNode<node::MaterialOutputNode>();
        node::Node* input = m_currentState->addNode<node::MaterialInputNode>(static_cast<asset::MaterialTypeAsset*>(m_selectedAsset));
        m_currentState->setLastNode(output);
        ImNodes::SetNodeEditorSpacePos(input->getId(), ImVec2(50, 50));
        ImNodes::SetNodeEditorSpacePos(output->getId(), ImVec2(350, 50));
    }
}

void NodeEditor::renderMaterialEditorContextMenu() {
    const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

    if (ImGui::MenuItem("Add"))
    {
        node::Node* n = m_currentState->addNode<node::AddNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::MenuItem("Sampler 2D"))
    {
        node::Node* n = m_currentState->addNode<node::Sampler2DNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }
}

void NodeEditor::renderRenderPipelineEditorContextMenu() {
    const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

    if (ImGui::MenuItem("Entities"))
    {
        node::Node* n = m_currentState->addNode<node::EntitiesNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::MenuItem("Camera provider"))
    {
        node::Node* n = m_currentState->addNode<node::CameraProviderNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::MenuItem("Target provider"))
    {
        node::Node* n = m_currentState->addNode<node::TargetProviderNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::BeginMenu("Editor"))
    {
        if (ImGui::MenuItem("Render"))
        {
            node::Node* n = m_currentState->addNode<node::EditorRenderNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        if (ImGui::MenuItem("Camera"))
        {
            node::Node* n = m_currentState->addNode<node::EditorCameraNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        if (ImGui::MenuItem("Target"))
        {
            node::Node* n = m_currentState->addNode<node::EditorTargetNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }


        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Convert"))
    {
        if (ImGui::MenuItem("Camera to Eye"))
        {
            node::Node* n = m_currentState->addNode<node::ConvertNode>(
                pipeline::getTypeName(pipeline::PropertyType::Camera), 
                pipeline::getTypeName(pipeline::PropertyType::Eye));
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Rendering"))
    {
        if (ImGui::MenuItem("Depth & Color"))
        {
            node::Node* n = m_currentState->addNode<node::RenderNode>();
            ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
        }

        ImGui::EndMenu();
    }
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
