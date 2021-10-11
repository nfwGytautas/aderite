#pragma once

#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/node/Forward.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Link between two pins
*/
class Link
{
public:
	Link(int id, InputPin* in, OutputPin* out);

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
	 * @brief Renders the link on the node editor
	*/
	void renderUI();
private:
	int m_id = 0;
	InputPin* m_inPin = nullptr;
	OutputPin* m_outPin = nullptr;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
