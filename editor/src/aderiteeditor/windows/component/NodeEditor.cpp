#include "NodeEditor.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/compiler/Compiler.hpp"

// Nodes
#include "aderiteeditor/node/MaterialInputNode.hpp"
#include "aderiteeditor/node/MaterialOutputNode.hpp"
#include "aderiteeditor/node/Sampler2DNode.hpp"
#include "aderiteeditor/node/AddNode.hpp"


#include <fstream>
#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

constexpr int c_RootMaterialNodeId = 0;

NodeEditor::NodeEditor() {}

NodeEditor::~NodeEditor() {}

void NodeEditor::init() {
    
}

void NodeEditor::render() {
    if (ImGui::Begin("Node editor")) {
        if (ImGui::Button("Compile")) {
            compiler::Compiler::compileGraph(m_currentState);
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
            ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
            ImNodes::BeginNodeEditor();

            m_currentState->renderUI();

            ImNodes::MiniMap();
            ImNodes::EndNodeEditor();
            ImNodes::PopAttributeFlag();

            {
                int start;
                int end;
                if (ImNodes::IsLinkCreated(&start, &end)) {
                    m_currentState->connect(start, end);
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

        m_currentState = new node::Graph();

        node::Node* n = m_currentState->addNode<node::MaterialOutputNode>();
        m_currentState->addNode<node::MaterialInputNode>(static_cast<asset::MaterialTypeAsset*>(m_selectedAsset));
        m_currentState->setLastNode(n);
    }
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
