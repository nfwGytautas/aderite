#include "InputPin.hpp"

#include "aderite/utility/Log.hpp"

#include "aderiteeditor/node/Link.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

namespace aderite {
namespace node {

InputPin::InputPin(Node* node, const std::string& type, const std::string& name) : m_node(node), m_type(type), m_name(name) {}

int InputPin::getId() const {
    return m_id;
}

void InputPin::setId(int id) {
    m_id = id;
}

std::string InputPin::getType() const {
    return m_type;
}

void InputPin::setType(const std::string& type) {
    m_type = type;
    disconnect();
}

std::string InputPin::getName() const {
    return m_name;
}

OutputPin* InputPin::getConnection() const {
    if (m_link == nullptr) {
        return nullptr;
    }

    return m_link->getOutputPin();
}

void InputPin::disconnect() {
    if (m_link) {
        m_link->destroy();
    }
}

compiler::EvaluatorValue InputPin::getValue() const {
    OutputPin* pin = getConnection();
    if (!pin) {
        return 0;
    } else {
        return pin->getValue();
    }
}

Node* InputPin::getNode() const {
    return m_node;
}

bool InputPin::isOptional() const {
    return m_optional;
}

void InputPin::setOptional(bool optional) {
    m_optional = optional;
}

} // namespace node
} // namespace aderite
