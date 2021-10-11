#include "Graph.hpp"

#include "aderite/utility/Log.hpp"
#include "aderiteeditor/node/Node.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/Link.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

Graph::Graph() {

}

Graph::~Graph() {
	for (Node* n : m_nodes) {
		delete n;
	}

	for (OutputPin* p : m_outputPins) {
		delete p;
	}

	for (InputPin* p : m_inputPins) {
		delete p;
	}
}

OutputPin* Graph::createOutputPin(const std::string& type, const std::string& name) {
	OutputPin* result = new OutputPin(m_nextId++, type, name);
	m_outputPins.push_back(result);
	return result;
}

InputPin* Graph::createInputPin(const std::string& type, const std::string& name) {
	InputPin* result = new InputPin(m_nextId++, type, name);
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

	Link* l = new Link(m_nextId++, ipin, opin);
	m_links[l->getId()] = l;
}

void Graph::disconnectLink(int linkId) {
	if (m_links.find(linkId) == m_links.end()) {
		return;
	}

	delete m_links[linkId];
	m_links.erase(linkId);
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
	return true;
}

bool Graph::deserialize(YAML::Node& data) {
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

ADERITE_EDITOR_NODE_NAMESPACE_END
