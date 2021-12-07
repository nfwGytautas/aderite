#include "NodeEditor.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/input/InputManager.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/platform/pc/backend/node/imnodes.h"
#include "aderiteeditor/platform/pc/backend/node/imnodes_aderite.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor {

constexpr int c_RootMaterialNodeId = 0;

NodeEditor::NodeEditor() {}

NodeEditor::~NodeEditor() {}

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
        io::SerializableObject* selectedObject = State::getInstance().getSelectedObject();
        node::Graph* graph = nullptr;

        if (selectedObject == nullptr) {
            ImGui::End();
            return;
        }

        if (selectedObject->getType() != static_cast<reflection::Type>(reflection::RuntimeTypes::MAT_TYPE)) {
            ImGui::End();
            return;
        }

        switch (static_cast<reflection::RuntimeTypes>(selectedObject->getType())) {
        case reflection::RuntimeTypes::MAT_TYPE: {
            graph = static_cast<asset::EditorMaterialType*>(selectedObject);
            break;
        }
        default: {
            ImGui::End();
            return;
        }
        }

        if (graph != nullptr) {
            bool pushed = false;
            if (aderite::Engine::getInputManager()->isKeyPressed(input::Key::LEFT_ALT)) {
                ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
                pushed = true;
            }
            ImNodes::BeginNodeEditor();

            // Context menu
            {
                const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImNodes::IsEditorHovered() &&
                                        ImGui::IsMouseDown(ImGuiMouseButton_Right);

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
                if (!ImGui::IsAnyItemHovered() && open_popup) {
                    ImGui::OpenPopup("add node");
                }

                if (ImGui::BeginPopup("add node")) {
                    const ImVec2 gridPos = ImNodes::ToEditorGridPos(ImGui::GetMousePosOnOpeningCurrentPopup());
                    node::Node* node = nullptr;

                    if (ImGui::MenuItem("Add")) {
                        node = new node::AddNode();
                    }

                    if (ImGui::BeginMenu("Sampler")) {
                        if (ImGui::MenuItem("2D")) {
                            node = new node::Sample2DTextureNode();
                        }

                        ImGui::EndMenu();
                    }

                    if (ImGui::BeginMenu("Vertex")) {
                        if (ImGui::MenuItem("UV")) {
                            node = new node::VertexUVProviderNode();
                        }

                        ImGui::EndMenu();
                    }

                    if (node != nullptr) {
                        node->setPosition({gridPos.x, gridPos.y});
                        graph->addNode(node);
                    }

                    ImGui::EndPopup();
                }
                ImGui::PopStyleVar();
            }

            graph->render();

            ImNodes::MiniMap();
            ImNodes::EndNodeEditor();

            if (pushed) {
                ImNodes::PopAttributeFlag();
            }

            // New link
            {
                int start;
                int end;
                if (ImNodes::IsLinkCreated(&start, &end)) {
                    node::OutNodePin* outPin = static_cast<node::OutNodePin*>(ImNodes::GetPinUserData(start));
                    node::InNodePin* inPin = static_cast<node::InNodePin*>(ImNodes::GetPinUserData(end));
                    inPin->setConnectedOutPin(outPin->getFullName());
                }
            }

            // Delete node
            {
                if (aderite::Engine::getInputManager()->isKeyPressed(input::Key::DEL)) {
                    std::vector<int> nodes;
                    nodes.resize(ImNodes::NumSelectedNodes());

                    if (nodes.size() > 0) {
                        ImNodes::GetSelectedNodes(nodes.data());

                        for (int nodeId : nodes) {
                            node::Node* node = static_cast<node::Node*>(ImNodes::GetNodeUserData(nodeId));
                            graph->removeNode(node);
                        }
                    }

                    ImNodes::ClearNodeSelection();
                }
            }

            // Delete link
            {
                int link_id;
                if (ImNodes::IsLinkDestroyed(&link_id)) {
                    node::InNodePin* inPin = static_cast<node::InNodePin*>(ImNodes::GetLinkUserData(link_id));
                    inPin->setConnectedOutPin("");
                }
            }
        }
    }

    ImGui::End();
}

} // namespace editor
} // namespace aderite
