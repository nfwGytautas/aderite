#include "Graph.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/Link.hpp"
#include "aderiteeditor/node/Node.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/platform/pc/backend/node/imnodes.h"
#include "aderiteeditor/runtime/EditorTypes.hpp"

namespace aderite {
namespace node {

Graph::Graph() {}

Graph::~Graph() {
    for (Node* n : m_nodes) {
        delete n;
    }

    for (auto pair : m_links) {
        delete pair.second;
    }
}

void Graph::connect(int outputPinId, int inputPinId) {
    OutputPin* opin = findOutputPin(outputPinId);
    InputPin* ipin = findInputPin(inputPinId);

    if (opin == nullptr || ipin == nullptr) {
        LOG_ERROR("Tried to connect invalid pins");
        return;
    }

    // Check if connection is aborted or not
    Node* inode = ipin->getNode();
    Node* onode = opin->getNode();

    if (!inode->onConnectToInput(ipin, opin)) {
        return;
    }

    if (!onode->onConnectToOutput(opin, ipin)) {
        return;
    }

    // Verify types
    if (ipin->getType() != opin->getType()) {
        // Incorrect types
        return;
    }

    if (ipin->getType() == asset::getNameForType(asset::PropertyType::NONE) ||
        opin->getType() == asset::getNameForType(asset::PropertyType::NONE)) {
        // None type pins
        return;
    }

    // Check if there already is a connection
    if (ipin->getConnection() != nullptr) {
        // Disconnect previous
        ipin->disconnect();
    }

    // Check if there are free links
    if (m_freeLinks.size() > 0) {
        // Free link
        int linkId = m_freeLinks.top();
        m_links[linkId]->setPins(*ipin, *opin);
        m_freeLinks.pop();
    } else {
        // Create new link
        Link* l = new Link(m_nextId++, this, ipin, opin);
        m_links[l->getId()] = l;
    }
}

void Graph::disconnectLink(int linkId) {
    if (m_links.find(linkId) == m_links.end()) {
        return;
    }

    if (m_links[linkId]->isValid()) {
        // Called by user
        m_links[linkId]->destroy();
    } else {
        m_freeLinks.push(linkId);
    }
}

void Graph::deleteNode(int id) {
    Node* n = findNode(id);

    if (n == nullptr) {
        LOG_WARN("Failed to delete node {0}", id);
        return;
    }

    // Disconnect
    for (InputPin* ipin : n->getInputPins()) {
        ipin->disconnect();
    }

    for (OutputPin* opin : n->getOutputPins()) {
        opin->disconnect();
    }

    // Delete node
    auto it = std::find_if(m_nodes.begin(), m_nodes.end(), [id](Node* n) {
        return n->getId() == id;
    });
    m_nodes.erase(it);
    delete n;
}

void Graph::setLastNode(Node* node) {
    m_lastNode = node;
}

Node* Graph::getLastNode() const {
    return m_lastNode;
}

void Graph::resetEvaluateFlag() const {
    for (Node* node : m_nodes) {
        node->resetEvaluateFlag();
    }
}

void Graph::renderUI() {
    for (Node* node : m_nodes) {
        if (node->getId() == -1) {
            node->setId(m_nextId++);
            node->setPosition(glm::vec2(0, 0));
        }

        ImNodes::BeginNode(node->getId());

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node->getNodeName());
        ImNodes::EndNodeTitleBar();

        node->renderBody();

        for (InputPin* i : node->getInputPins()) {
            if (i->getId() == -1) {
                i->setId(m_nextId++);
            }

            ImNodes::BeginInputAttribute(i->getId());
            ImGui::Text(i->getName().c_str());
            ImGui::SameLine();
            ImGui::Text(i->getType().c_str());
            ImNodes::EndInputAttribute();
        }

        for (OutputPin* o : node->getOutputPins()) {
            if (o->getId() == -1) {
                o->setId(m_nextId++);
            }

            ImNodes::BeginOutputAttribute(o->getId());
            ImGui::Text(o->getName().c_str());
            ImGui::SameLine();
            ImGui::Text(o->getType().c_str());
            ImNodes::EndOutputAttribute();
        }

        ImVec2 npos = ImNodes::GetNodeGridSpacePos(node->getId());
        node->setPosition({npos.x, npos.y});

        ImNodes::EndNode();
    }

    for (auto link : m_links) {
        link.second->renderUI();
    }
}

void Graph::prepareToDisplay() {
    for (Node* n : m_nodes) {
        n->prepareToDisplay();
    }
}

void Graph::closingDisplay() {
    for (Node* n : m_nodes) {
        n->closingDisplay();
    }
}

OutputPin* Graph::findOutputPin(int id) const {
    for (Node* n : m_nodes) {
        for (OutputPin* pin : n->getOutputPins()) {
            if (pin->getId() == id) {
                return pin;
            }
        }
    }
    return nullptr;
}

InputPin* Graph::findInputPin(int id) const {
    for (Node* n : m_nodes) {
        for (InputPin* pin : n->getInputPins()) {
            if (pin->getId() == id) {
                return pin;
            }
        }
    }
    return nullptr;
}

Node* Graph::findNode(int id) const {
    auto it = std::find_if(m_nodes.begin(), m_nodes.end(), [id](Node* n) {
        return n->getId() == id;
    });

    if (it == m_nodes.end()) {
        return nullptr;
    }

    return *it;
}

reflection::Type Graph::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::GraphAsset);
}

bool Graph::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "NextId" << YAML::Value << m_nextId;

    emitter << YAML::Key << "LastNode" << YAML::Value;
    if (m_lastNode != nullptr) {
        emitter << m_lastNode->getId();
    } else {
        emitter << YAML::Null;
    }

    emitter << YAML::Key << "Nodes" << YAML::BeginSeq;
    for (Node* node : m_nodes) {
        serializer->writeObject(emitter, node);
    }
    emitter << YAML::EndSeq;

    emitter << YAML::Key << "Links" << YAML::BeginSeq;
    for (auto pair : m_links) {
        if (!pair.second->isValid()) {
            continue;
        }

        emitter << YAML::BeginMap;
        emitter << YAML::Key << "Out" << YAML::Value << pair.second->getOutputPin()->getId();
        emitter << YAML::Key << "In" << YAML::Value << pair.second->getInputPin()->getId();
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;

    return true;
}

bool Graph::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Nodes
    for (const YAML::Node& node : data["Nodes"]) {
        Node* n = static_cast<Node*>(serializer->parseObject(node));
        m_nodes.push_back(n);
    }

    // Links
    for (const YAML::Node& link : data["Links"]) {
        int outId = link["Out"].as<int>();
        int inId = link["In"].as<int>();
        connect(outId, inId);
    }

    // Last because nodes will create ids
    m_nextId = data["NextId"].as<int>();

    if (!data["LastNode"].IsNull()) {
        m_lastNode = findNode(data["LastNode"].as<int>());
    }

    return true;
}

} // namespace node
} // namespace aderite
