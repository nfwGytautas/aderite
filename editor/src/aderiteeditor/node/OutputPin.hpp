#pragma once

#include <string>
#include <vector>
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/node/Forward.hpp"
#include "aderiteeditor/compiler/Forward.hpp"
#include "aderiteeditor/compiler/GraphEvaluator.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Output pin for node editor
*/
class OutputPin 
{
public:
	OutputPin(int id, Node* node, const std::string& type, const std::string& name);

	/**
	 * @brief Returns the id of the pin
	*/
	int getId() const;

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
	 * @brief Disconnects all pins connected to this one
	*/
	void disconnect();

	/**
	 * @brief Renders the pin
	*/
	void renderUI();

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
	int m_id;
	std::string m_type;
	std::string m_name;
	Node* m_node = nullptr;
	compiler::EvaluatorValue m_value;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
