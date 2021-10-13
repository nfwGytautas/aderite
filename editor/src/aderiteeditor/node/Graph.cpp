#include "Graph.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/property/Property.hpp"
#include "aderiteeditor/node/Node.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/Link.hpp"

#include "aderiteeditor/node/ConvertNode.hpp"

#include "aderiteeditor/node/AddNode.hpp"
#include "aderiteeditor/node/MaterialInputNode.hpp"
#include "aderiteeditor/node/MaterialOutputNode.hpp"
#include "aderiteeditor/node/Sampler2DNode.hpp"

#include "aderiteeditor/node/pipeline/EntitiesNode.hpp"
#include "aderiteeditor/node/pipeline/CameraProviderNode.hpp"
#include "aderiteeditor/node/pipeline/ScreenNode.hpp"
#include "aderiteeditor/node/pipeline/RenderNode.hpp"
#include "aderiteeditor/node/pipeline/TargetProviderNode.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

Graph::Graph() {

}

Graph::~Graph() {
	for (Node* n : m_nodes) {
		delete n;
	}

	for (auto pair : m_links) {
		delete pair.second;
	}
}

OutputPin* Graph::createOutputPin(Node* node, const std::string& type, const std::string& name) {
	OutputPin* result = new OutputPin(m_nextId++, node, type, name);
	m_outputPins.push_back(result);
	return result;
}

InputPin* Graph::createInputPin(Node* node, const std::string& type, const std::string& name) {
	InputPin* result = new InputPin(m_nextId++, node, type, name);
	m_inputPins.push_back(result);
	return result;
}

void Graph::connect(int outputPinId, int inputPinId) {
	OutputPin* opin = findOutputPin(outputPinId);
	InputPin* ipin = findInputPin(inputPinId);

	if (opin == nullptr || ipin == nullptr) {
		LOG_ERROR("Tried to connect invalid pins");
		return;
	}

	if (ipin->getType() != opin->getType()) {
		// Incorrect types
		return;
	}

	if (ipin->getType() == asset::prop::getNameForType(asset::prop::PropertyType::NONE) ||
		opin->getType() == asset::prop::getNameForType(asset::prop::PropertyType::NONE)) {
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
	}
	else {
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
	}
	else {
		m_freeLinks.push(linkId);
	}
}

void Graph::deleteNode(int id) {
	Node* n = findNode(id);

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
		node->renderUI();
	}

	for (auto link : m_links) {
		link.second->renderUI();
	}
}

bool Graph::serialize(YAML::Emitter& out) {
	out << YAML::Key << "NextId" << YAML::Value << m_nextId;

	out << YAML::Key << "LastNode" << YAML::Value;
	if (m_lastNode != nullptr) {
		out << m_lastNode->getId();
	}
	else {
		out << YAML::Null;
	}

	out << YAML::Key << "Nodes" << YAML::BeginSeq;
	for (Node* node : m_nodes) {
		node->serialize(out);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "Links" << YAML::BeginSeq;
	for (auto pair : m_links) {
		if (!pair.second->isValid()) {
			continue;
		}

		out << YAML::BeginMap;
		out << YAML::Key << "Out" << YAML::Value << pair.second->getOutputPin()->getId();
		out << YAML::Key << "In" << YAML::Value << pair.second->getInputPin()->getId();
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	return true;
}

bool Graph::deserialize(YAML::Node& data) {
	// Nodes
	for (YAML::Node& node : data["Nodes"]) {
		std::string type = node["NodeType"].as<std::string>();
		
		Node* n = nullptr;
		if (type == "MaterialInput") {
			asset::MaterialTypeAsset* mta = static_cast<asset::MaterialTypeAsset*>(::aderite::Engine::getAssetManager()->getOrRead(node["Material"].as<std::string>()));
			n = addNode<node::MaterialInputNode>(mta);
		}
		else if (type == "Add") {
			n = addNode<node::AddNode>();
		}
		else if (type == "Sampler2D") {
			n = addNode<node::Sampler2DNode>();
		}
		else if (type == "MaterialOutput") {
			n = addNode<node::MaterialOutputNode>();
		}
		else if (type == "Convert") {
			std::string from = node["From"].as<std::string>();
			std::string to = node["To"].as<std::string>();
			n = addNode<node::ConvertNode>(from, to);
		}
		else if (type == "Target") {
			n = addNode<node::TargetProviderNode>();
		}
		else if (type == "Screen") {
			n = addNode<node::ScreenNode>();
		}
		else if (type == "Render") {
			n = addNode<node::RenderNode>();
		}
		else if (type == "Entities") {
			n = addNode<node::EntitiesNode>();
		}
		else if (type == "Camera") {
			n = addNode<node::CameraProviderNode>();
		}

		n->deserialize(node);
	}

	// Links
	for (YAML::Node& link : data["Links"]) {
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

OutputPin* Graph::findOutputPin(int id) const {
	auto it = std::find_if(m_outputPins.begin(), m_outputPins.end(), [id](OutputPin* p) {
		return p->getId() == id;
	});

	if (it == m_outputPins.end()) {
		return nullptr;
	}

	return *it;
}

InputPin* Graph::findInputPin(int id) const {
	auto it = std::find_if(m_inputPins.begin(), m_inputPins.end(), [id](InputPin* p) {
		return p->getId() == id;
	});

	if (it == m_inputPins.end()) {
		return nullptr;
	}

	return *it;
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

ADERITE_EDITOR_NODE_NAMESPACE_END
