#include "Link.hpp"

#include "aderite/utility/Log.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

Link::Link(int id, InputPin* in, OutputPin* out)
	: m_id(id), m_inPin(in), m_outPin(out)
{}

int Link::getId() const {
	return m_id;
}

InputPin* Link::getInputPin() const {
	return m_inPin;
}

OutputPin* Link::getOutputPin() const {
	return m_outPin;
}

void Link::setInputPin(InputPin& pin) {
	m_inPin = &pin;
}

void Link::setOutputPin(OutputPin& pin) {
	m_outPin = &pin;
}

void Link::renderUI() {
	ImNodes::Link(m_id, m_outPin->getId(), m_inPin->getId());
}

ADERITE_EDITOR_NODE_NAMESPACE_END
