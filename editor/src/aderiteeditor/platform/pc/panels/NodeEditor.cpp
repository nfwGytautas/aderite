#include "NodeEditor.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/platform/pc/backend/node/imnodes.h"
#include "aderiteeditor/platform/pc/backend/node/imnodes_aderite.hpp"
#include "aderiteeditor/shared/State.hpp"

//// Nodes
//#include "aderiteeditor/node/material/AddNode.hpp"
//#include "aderiteeditor/node/material/MaterialInputNode.hpp"
//#include "aderiteeditor/node/material/MaterialOutputNode.hpp"
//#include "aderiteeditor/node/material/Sampler2DNode.hpp"
//
//// Shared
//#include "aderiteeditor/node/shared/ConvertNode.hpp"
//#include "aderiteeditor/node/shared/Properties.hpp"

namespace aderite {
namespace editor {

class TestNode : public node::Node {
public:
    TestNode() {
        p_inPins.push_back(node::InNodePin(this, node::PinType::Float));
        p_outPins.push_back(node::OutNodePin(this, node::PinType::Float));
    }

    // Inherited via Node
    virtual const char* getTypeName() const override {
        return "TestNode";
    }
};

constexpr int c_RootMaterialNodeId = 0;

NodeEditor::NodeEditor() {
    m_graph = new node::Graph();
}

NodeEditor::~NodeEditor() {}

void NodeEditor::setGraph(node::Graph* graph) {
    /*if (m_graph != nullptr) {
        m_graph->closingDisplay();
    }

    if (graph != nullptr) {
        graph->prepareToDisplay();
    }*/

    m_graph = graph;
}

void NodeEditor::renderMaterialEditorContextMenu() {
    const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

    if (ImGui::MenuItem("Add")) {
        m_graph->addNode(new TestNode());
        //ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    /*if (ImGui::MenuItem("Add")) {
        node::Node* n = m_graph->addNode<node::AddNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }

    if (ImGui::MenuItem("Sampler 2D")) {
        node::Node* n = m_graph->addNode<node::Sampler2DNode>();
        ImNodes::SetNodeScreenSpacePos(n->getId(), click_pos);
    }*/
}

bool NodeEditor::init() {
    return true;
}

void NodeEditor::shutdown() {}

void NodeEditor::render() {
    if (!this->isProjectLoaded()) {
        // No project so can't render this
        return;
    }

    if (ImGui::Begin("Node editor")) {
        if (m_graph) {
            bool pushed = false;
            if (aderite::Engine::getInputManager()->isKeyPressed(input::Key::LEFT_ALT)) {
                ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
                pushed = true;
            }
            ImNodes::BeginNodeEditor();

            {
                const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImNodes::IsEditorHovered() &&
                                        ImGui::IsMouseDown(ImGuiMouseButton_Right);

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
                if (!ImGui::IsAnyItemHovered() && open_popup) {
                    ImGui::OpenPopup("add node");
                }

                if (ImGui::BeginPopup("add node")) {
                    renderMaterialEditorContextMenu();
                    ImGui::EndPopup();
                }
                ImGui::PopStyleVar();
            }

            m_graph->render();

            ImNodes::MiniMap();
            ImNodes::EndNodeEditor();

            if (pushed) {
                ImNodes::PopAttributeFlag();
            }

            {
                int start;
                int end;
                if (ImNodes::IsLinkCreated(&start, &end)) {
                    node::OutNodePin* outPin = static_cast<node::OutNodePin*>(ImNodes::GetPinUserData(start));
                    node::InNodePin* inPin = static_cast<node::InNodePin*>(ImNodes::GetPinUserData(end));
                    inPin->setConnectedOutPin(outPin);
                }
            }

            {
                if (aderite::Engine::getInputManager()->isKeyPressed(input::Key::DEL)) {
                    std::vector<int> nodes;
                    nodes.resize(ImNodes::NumSelectedNodes());

                    if (nodes.size() > 0) {
                        ImNodes::GetSelectedNodes(nodes.data());

                        for (int nodeId : nodes) {
                            node::Node* node = static_cast<node::Node*>(ImNodes::GetNodeUserData(nodeId));
                            m_graph->removeNode(node);
                        }
                    }

                    ImNodes::ClearNodeSelection();
                }
            }

            {
                int link_id;
                if (ImNodes::IsLinkDestroyed(&link_id)) {
                    node::InNodePin* inPin = static_cast<node::InNodePin*>(ImNodes::GetLinkUserData(link_id));
                    inPin->setConnectedOutPin(nullptr);
                }
            }
        }
    }

    ImGui::End();
}

} // namespace editor
} // namespace aderite
