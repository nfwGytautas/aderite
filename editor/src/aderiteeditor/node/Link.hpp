#pragma once

#include "aderiteeditor/node/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace node {

/**
 * @brief Link between two pins
 */
class Link {
public:
    Link(int id, Graph* graph, InputPin* in, OutputPin* out);

    /**
     * @brief Returns the link id
     */
    int getId() const;

    /**
     * @brief Returns the input pin of the link
     */
    InputPin* getInputPin() const;

    /**
     * @brief Returns the output pin of the link
     */
    OutputPin* getOutputPin() const;

    /**
     * @brief Sets the input pin of the link
     */
    void setInputPin(InputPin& pin);

    /**
     * @brief Sets the output pin of the link
     */
    void setOutputPin(OutputPin& pin);

    /**
     * @brief Sets input and output pins of the link
     * @param iPin Input pin
     * @param oPin Output pin
     */
    void setPins(InputPin& iPin, OutputPin& oPin);

    /**
     * @brief Destroys this link
     */
    void destroy();

    /**
     * @brief Returns true if the link is valid, false otherwise
     */
    bool isValid() const;

    /**
     * @brief Renders the link on the node editor
     */
    void renderUI();

private:
    int m_id = 0;
    Graph* m_graph = nullptr;
    InputPin* m_inPin = nullptr;
    OutputPin* m_outPin = nullptr;
};

} // namespace node
} // namespace aderite
