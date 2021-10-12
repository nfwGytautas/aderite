#pragma once

#include <string>
#include <vector>
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/node/Forward.hpp"
#include "aderiteeditor/compiler/Forward.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Input pin for node editor
*/
class InputPin
{
public:
	InputPin(int id, Node* node, const std::string& type, const std::string& name);

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
	 * @brief Renders the pin
	*/
	void renderUI();

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
	compiler::Variable getValue() const;

	/**
	 * @brief Returns the node that this pin exists on
	 * @return Node instance
	*/
	Node* getNode() const;
private:
	Link* m_link = nullptr;
	friend class Link;
	friend class Node;
private:
	int m_id;
	std::string m_type;
	std::string m_name;
	Node* m_node = nullptr;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
