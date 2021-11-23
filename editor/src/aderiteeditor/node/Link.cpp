#include "Link.hpp"

#include "aderite/utility/Log.hpp"

#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/platform/pc/backend/node/imnodes.h"

namespace aderite {
namespace node {

Link::Link(int id, Graph* graph, InputPin* in, OutputPin* out) : m_id(id), m_graph(graph), m_inPin(in), m_outPin(out) {
    m_inPin->m_link = this;
    m_outPin->m_links.push_back(this);
}

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
    m_inPin->m_link = this;
}

void Link::setOutputPin(OutputPin& pin) {
    m_outPin = &pin;
    m_outPin->m_links.push_back(this);
}

void Link::setPins(InputPin& iPin, OutputPin& oPin) {
    setInputPin(iPin);
    setOutputPin(oPin);
}

void Link::destroy() {
    if (m_inPin != nullptr && m_outPin != nullptr) {
        m_inPin->m_link = nullptr;
        m_outPin->m_links.erase(std::find(m_outPin->m_links.begin(), m_outPin->m_links.end(), this));
        m_inPin = nullptr;
        m_outPin = nullptr;
        m_graph->disconnectLink(this->m_id);
    }
}

bool Link::isValid() const {
    return m_inPin != nullptr && m_outPin != nullptr;
}

void Link::renderUI() {
    if (isValid()) {
        ImNodes::Link(m_id, m_outPin->getId(), m_inPin->getId());
    }
}

} // namespace node
} // namespace aderite
