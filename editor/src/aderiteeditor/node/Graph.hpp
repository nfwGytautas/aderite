#pragma once

#include <stack>
#include <vector>
#include <unordered_map>
#include "aderite/interfaces/ISerializable.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/compiler/Forward.hpp"
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
	 * @param node Node where the pin will be placed in
	 * @param type Output pin type
	 * @param name Output pin name
	 * @return OutputPin instance
	*/
	OutputPin* createOutputPin(Node* node, const std::string& type, const std::string& name);

	/**
	 * @brief Creates a input pin with given arguments
	 * @param node Node where the pin will be placed in
	 * @param type Input pin type
	 * @param name Input pin name
	 * @return InputPin instance
	*/
	InputPin* createInputPin(Node* node, const std::string& type, const std::string& name);

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
	 * @brief Delete node from graph
	 * @param id Id of the node to delete
	*/
	void deleteNode(int id);

	/**
	 * @brief Sets the last node of the graph
	 * @param node Node to make last
	*/
	void setLastNode(Node* node);

	/**
	 * @brief Returns the last node of the graph
	 * @return Node instance
	*/
	Node* getLastNode() const;

	/**
	 * @brief Resets evaluate flag on all nodes
	*/
	void resetEvaluateFlag() const;

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
	Node* findNode(int id) const;
private:
	int m_nextId = 0;

	Node* m_lastNode = nullptr;

	std::vector<Node*> m_nodes;
	std::vector<OutputPin*> m_outputPins;
	std::vector<InputPin*> m_inputPins;

	// Links
	std::unordered_map<int, Link*> m_links;
	std::stack<int> m_freeLinks;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
