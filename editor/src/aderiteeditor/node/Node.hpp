#pragma once

#include "aderite/interfaces/ISerializable.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/node/Forward.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Base node class
 * NOTES: Inherited nodes should define their constructor as (id, Graph*, args...)
*/
class Node 
{
public:
	Node(int id);
	virtual ~Node();

	/**
	 * @brief Returns the node id
	*/
	int getId() const;

	/**
	 * @brief Returns the input pins of the node
	*/
	std::vector<InputPin*> getInputPins() const;

	/**
	 * @brief Returns the output pins of the node
	*/
	std::vector<OutputPin*> getOutputPins() const;

	/**
	 * @brief Render UI for this node
	*/
	virtual void renderUI() = 0;

protected:
	/**
	 * @brief Renders input and output pins
	*/
	void renderPins();
protected:
	std::vector<InputPin*> p_inputs;
	std::vector<OutputPin*> p_outputs;
private:
	int m_id = -1;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
