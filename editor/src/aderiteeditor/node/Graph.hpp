#pragma once

#include <vector>
#include <unordered_map>
#include "aderite/interfaces/ISerializable.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/node/Forward.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Main node editor object which represent the current graph state
*/
class Graph : public interfaces::ISerializable
{
public:
	Graph();
	virtual ~Graph();

	/**
	 * @brief Creates a output pin with given arguments
	 * @param type Output pin type
	 * @param name Output pin name
	 * @return OutputPin instance
	*/
	OutputPin* createOutputPin(const std::string& type, const std::string& name);

	/**
	 * @brief Creates a input pin with given arguments
	 * @param type Input pin type
	 * @param name Input pin name
	 * @return InputPin instance
	*/
	InputPin* createInputPin(const std::string& type, const std::string& name);

	/**
	 * @brief Connects a output pin to an input pin
	 * @param outputPinId Output pin id
	 * @param inputPinId Input pin id
	*/
	void connect(int outputPinId, int inputPinId);

	/**
	 * @brief Disconnect link id
	 * @param linkId Link id
	*/
	void disconnectLink(int linkId);

	/**
	 * @brief Adds a new node to the graph
	 * @tparam T Graph type
	 * @return Node instance
	*/
	template<typename T, class ...Args>
	T* addNode(Args&&... args) {
		T* node = new T(m_nextId++, this, std::forward<Args>(args)...);
		m_nodes.push_back(static_cast<Node*>(node));
		return node;
	}

	/**
	 * @brief Render the graph
	*/
	void renderUI();

	// Inherited via ISerializable
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;
private:
	OutputPin* findOutputPin(int id) const;
	InputPin* findInputPin(int id) const;
private:
	int m_nextId = 0;
	std::vector<Node*> m_nodes;
	std::vector<OutputPin*> m_outputPins;
	std::vector<InputPin*> m_inputPins;
	std::unordered_map<int, Link*> m_links;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
