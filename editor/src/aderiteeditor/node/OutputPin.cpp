#include "OutputPin.hpp"

#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/Link.hpp"

namespace aderite {
namespace node {

OutputPin::OutputPin(Node* node, const std::string& type, const std::string& name) : m_node(node), m_type(type), m_name(name) {}

int OutputPin::getId() const {
    return m_id;
}

void OutputPin::setId(int id) {
    m_id = id;
}

std::string OutputPin::getType() const {
    return m_type;
}

void OutputPin::setType(const std::string& type) {
    m_type = type;
    disconnect();
}

std::string OutputPin::getName() const {
    return m_name;
}

void OutputPin::setName(const std::string& name) {
    m_name = name;
}

void OutputPin::disconnect() {
    for (Link* l : m_links) {
        l->destroy();
    }

    m_links.clear();
}

void OutputPin::setValue(compiler::EvaluatorValue value) {
    m_value = value;
}

compiler::EvaluatorValue OutputPin::getValue() const {
    return m_value;
}

Node* OutputPin::getNode() const {
    return m_node;
}

} // namespace node
} // namespace aderite
