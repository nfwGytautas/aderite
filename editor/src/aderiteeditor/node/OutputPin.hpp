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
 * @brief Output pin for node editor
 */
class OutputPin {
public:
    OutputPin(Node* node, const std::string& type, const std::string& name);

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
     * @brief Sets the name of the output pin
     * @param name Name of the pin
     */
    void setName(const std::string& name);

    /**
     * @brief Disconnects all pins connected to this one
     */
    void disconnect();

    /**
     * @brief Set the value of the output pin
     * @param value New value of the pin
     */
    void setValue(compiler::EvaluatorValue value);

    /**
     * @brief Get the value of the output pin
     * @return Value
     */
    compiler::EvaluatorValue getValue() const;

    /**
     * @brief Returns the node that this pin exists on
     * @return Node instance
     */
    Node* getNode() const;

private:
    std::vector<Link*> m_links;
    friend class Link;
    friend class Node;

private:
    int m_id = -1;
    std::string m_type;
    std::string m_name;
    Node* m_node = nullptr;
    compiler::EvaluatorValue m_value;
};

} // namespace node
} // namespace aderite
