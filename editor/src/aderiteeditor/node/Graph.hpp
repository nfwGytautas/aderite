#pragma once

#include <stack>
#include <vector>
#include <unordered_map>
#include "aderite/io/SerializableObject.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/compiler/Forward.hpp"
#include "aderiteeditor/node/Forward.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Type of the graph
*/
enum class GraphType {
	RENDER_PIPELINE,
	MATERIAL
};

/**
 * @brief Main node editor object which represent the current graph state
*/
class Graph : public io::ISerializable
{
public:
	Graph();
	virtual ~Graph();

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
	template<typename T>
	T* addNode() {
		T* node = new T();
		m_nodes.push_back(static_cast<Node*>(node));
		node->setId(m_nextId++);
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
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
private:
	OutputPin* findOutputPin(int id) const;
	InputPin* findInputPin(int id) const;
	Node* findNode(int id) const;
private:
	int m_nextId = 0;

	Node* m_lastNode = nullptr;

	std::vector<Node*> m_nodes;

	// Links
	std::unordered_map<int, Link*> m_links;
	std::stack<int> m_freeLinks;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
