#include "Node.hpp"

#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

Node::Node(int id)
	: m_id(id)
{}

Node::~Node() {

}

int Node::getId() const {
	return m_id;
}

std::vector<InputPin*> Node::getInputPins() const {
	return p_inputs;
}

std::vector<OutputPin*> Node::getOutputPins() const {
	return p_outputs;
}

void Node::renderPins() {
	for (InputPin* i : p_inputs) {
		i->renderUI();
	}

	for (OutputPin* o : p_outputs) {
		o->renderUI();
	}
}

ADERITE_EDITOR_NODE_NAMESPACE_END
