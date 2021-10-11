#pragma once

#include <string>
#include <vector>
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/node/Forward.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Input pin for node editor
*/
class InputPin
{
public:
	InputPin(int id, const std::string& type, const std::string& name);

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
	 * @brief Returns the name of the pin
	 * @return Name of the pin
	*/
	std::string getName() const;

	/**
	 * @brief Renders the pin
	*/
	void renderUI();
private:
	int m_id;
	std::string m_type;
	std::string m_name;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
