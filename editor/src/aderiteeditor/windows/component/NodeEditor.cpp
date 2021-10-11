#include "NodeEditor.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/node/MaterialInputNode.hpp"
#include "aderiteeditor/node/Sampler2DNode.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

constexpr int c_RootMaterialNodeId = 0;

NodeEditor::NodeEditor() {}

NodeEditor::~NodeEditor() {}

void NodeEditor::init() {

}

void NodeEditor::render() {
    if (ImGui::Begin("Node editor")) {
        ImNodes::BeginNodeEditor();
        ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

        m_currentState.renderUI();

        ImNodes::MiniMap();
        ImNodes::EndNodeEditor();

        {
            int start;
            int end;
            if (ImNodes::IsLinkCreated(&start, &end)) {
                m_currentState.connect(start, end);
            }
        }

        {
            int link_id;
            if (ImNodes::IsLinkDestroyed(&link_id)) {
                m_currentState.disconnectLink(link_id);
            }
        }

        ImNodes::PopAttributeFlag();
    }

    ImGui::End();
}

void NodeEditor::shutdown() {

}

void NodeEditor::setActiveAsset(asset::Asset* asset) {
    m_selectedAsset = asset;

    if (m_selectedAsset->type() == asset::AssetType::MATERIAL_TYPE) {
        m_currentState.addNode<node::MaterialInputNode>(static_cast<asset::MaterialTypeAsset*>(m_selectedAsset));

        // TESTING
        m_currentState.addNode<node::Sampler2DNode>();
    }
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
