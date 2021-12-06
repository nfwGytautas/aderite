#include "GraphX.hpp"

#include "aderite/utility/Macros.hpp"
#include "aderite/utility/Random.hpp"

#include "aderiteeditor/platform/pc/backend/node/imnodes.h"
#include "aderiteeditor/platform/pc/backend/node/imnodes_aderite.hpp"

namespace aderite {
namespace node {

static constexpr const char* c_PinTypeMap[] = {"None", "Float"};

void Graph::addNode(Node* node) {
    m_nodes.push_back(node);
}

void Graph::removeNode(Node* node) {
    m_nodes.erase(std::find(m_nodes.begin(), m_nodes.end(), node));
}

const std::vector<Node*>& Graph::getNodes() const {
    return m_nodes;
}

void Graph::render() {
    // Nodes
    for (Node* node : m_nodes) {
        const std::string nodeName = node->getName();
        int id = this->getId(nodeName);

        // Render node
        ImNodes::BeginNode(id);
        ImNodes::SetNodeUserData(id, node);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node->getTypeName());
        ImNodes::EndNodeTitleBar();

        // Render pins
        for (InNodePin& ipin : node->getInPins()) {
            int pinId = this->getId(ipin.getFullName());

            ImNodes::BeginInputAttribute(pinId);
            ImNodes::SetPinUserData(pinId, &ipin);
            ImGui::Text(ipin.getPinName().c_str());
            ImGui::SameLine();
            ImGui::Text("(%s)", c_PinTypeMap[static_cast<size_t>(ipin.getType())]);
            ImNodes::EndInputAttribute();
        }

        for (OutNodePin& opin : node->getOutPins()) {
            int pinId = this->getId(opin.getFullName());

            ImNodes::BeginOutputAttribute(pinId);
            ImNodes::SetPinUserData(pinId, &opin);
            ImGui::Text(opin.getPinName().c_str());
            ImGui::SameLine();
            ImGui::Text("(%s)", c_PinTypeMap[static_cast<size_t>(opin.getType())]);
            ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
    }

    // Links
    for (Node* node : m_nodes) {
        for (InNodePin& ipin : node->getInPins()) {
            int ipinId = this->getId(ipin.getFullName());

            const OutNodePin* opin = ipin.getConnectOutPin();
            if (opin != nullptr) {
                if (ipin.getType() != opin->getType()) {
                    // Disconnect, type mismatch
                    ipin.setConnectedOutPin(nullptr);
                } else {
                    // Render connection
                    int opinId = this->getId(opin->getFullName());
                    ImNodes::Link(ipinId, opinId, ipinId);
                    ImNodes::SetLinkUserData(ipinId, &ipin);
                }
            }
        }
    }
}

int Graph::getId(const std::string& name) {
    ADERITE_DYNAMIC_ASSERT(!name.empty(), "Empty name passed to getId");

    auto it = m_idMap.find(name);

    if (it == m_idMap.end()) {
        m_idMap[name] = m_nextId++;
    }

    return m_idMap[name];
}

OutNodePin::OutNodePin(Node* node, PinType type) : m_node(node), m_type(type), m_name(utility::generateString(12)) {}

const std::string& OutNodePin::getPinName() const {
    return m_name;
}

std::string OutNodePin::getFullName() const {
    return m_node->getName() + "_" + m_name;
}

void OutNodePin::setType(PinType type) {
    m_type = type;
}

PinType OutNodePin::getType() const {
    return m_type;
}

InNodePin::InNodePin(Node* node, PinType type) : m_node(node), m_type(type), m_name(utility::generateString(12)) {}

const std::string& InNodePin::getPinName() const {
    return m_name;
}

std::string InNodePin::getFullName() const {
    return m_node->getName() + "_" + m_name;
}

void InNodePin::setType(PinType type) {}

PinType InNodePin::getType() const {
    return m_type;
}

OutNodePin* InNodePin::getConnectOutPin() const {
    return m_outNode;
}

void InNodePin::setConnectedOutPin(OutNodePin* out) {
    m_outNode = out;
}

Node::Node() : m_name(utility::generateString(12)) {}

const std::string& Node::getName() const {
    return m_name;
}

std::vector<InNodePin>& Node::getInPins() {
    return p_inPins;
}

std::vector<OutNodePin>& Node::getOutPins() {
    return p_outPins;
}

} // namespace node
} // namespace aderite
