#pragma once

#include <string>
#include <vector>

#include "aderiteeditor/compiler/Forward.hpp"
#include "aderiteeditor/compiler/GraphEvaluator.hpp"
#include "aderiteeditor/node/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace node {

/**
 * @brief Input pin for node editor
 */
class InputPin {
public:
    InputPin(Node* node, const std::string& type, const std::string& name);

    /**
     * @brief Returns the id of the pin
     */
    int getId() const;

    /**
     * @brief Sets the pin id
     * @param id New id of the pin
     */
    void setId(int id);

    /**
     * @brief Returns the type of the pin
     * @return Type as string
     */
    std::string getType() const;

    /**
     * @brief Changes the type of the pin, this disconnects any connections
     * @param type New type
     */
    void setType(const std::string& type);

    /**
     * @brief Returns the name of the pin
     * @return Name of the pin
     */
    std::string getName() const;

    /**
     * @brief Gets the output pin of that this pin is connected to or nullptr if no connection
     */
    OutputPin* getConnection() const;

    /**
     * @brief Disconnects this input pin from output pin
     */
    void disconnect();

    /**
     * @brief Returns the value of the input pin
     */
    compiler::EvaluatorValue getValue() const;

    /**
     * @brief Returns the node that this pin exists on
     * @return Node instance
     */
    Node* getNode() const;

    /**
     * @brief Returns true if this input pin is optional, false otherwise
     */
    bool isOptional() const;

    /**
     * @brief Sets the pin to be optional or not
     */
    void setOptional(bool optional);

private:
    Link* m_link = nullptr;
    friend class Link;
    friend class Node;
    friend class Graph;

private:
    int m_id = -1;
    std::string m_type;
    std::string m_name;
    Node* m_node = nullptr;
    bool m_optional = false;
};

} // namespace node
} // namespace aderite
