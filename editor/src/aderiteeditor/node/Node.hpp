#pragma once

#include "aderite/io/SerializableObject.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/node/Forward.hpp"
#include "aderiteeditor/compiler/Forward.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Base node class
 * NOTES: Inherited nodes should define their constructor as (id, Graph*, args...)
*/
class Node : public io::SerializableObject
{
public:
	Node();
	Node(int id);
	virtual ~Node();

	/**
	 * @brief Returns the node id
	*/
	int getId() const;

	/**
	 * @brief Sets the id of the node
	 * @param id New id of the node
	*/
	void setId(int id);

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
	void renderUI();

	/**
	 * @brief Evaluate this node data and store information in it's output pins
	*/
	virtual void evaluate(compiler::GraphEvaluator* evaluator);

	/**
	 * @brief Evaluates dependencies for this node
	*/
	void evaluateDependencies(compiler::GraphEvaluator* evaluator);

	/**
	 * @brief Resets evaluate flag on node
	*/
	void resetEvaluateFlag();

	/**
	 * @brief Function invoked when a new link is trying to be established to this node input pin
	 * @param target Input pin of this node
	 * @param source Output pin that is being connected from
	 * @return True if connection accepted, false otherwise
	*/
	virtual bool onConnectToInput(InputPin* target, OutputPin* source) { return true; };

	/**
	 * @brief Function invoked when a new link is trying to be established to this node output pin
	 * @param target Output pin of this node
	 * @param source Input pin that is being connected from
	 * @return True if connection accepted, false otherwise
	*/
	virtual bool onConnectToOutput(OutputPin* target, InputPin* source) { return true; };

	/**
	 * @brief Returns the name of the node
	 * @return Name of the node
	*/
	virtual const char* getNodeName() const = 0;

	/**
	 * @brief Optional UI body
	*/
	virtual void renderBody() {};
protected:
	/**
	 * @brief Serializes pins and position
	*/
	void serializeData(YAML::Emitter& out);

	/**
	 * @brief Deserializes pins and position
	*/
	void deserializeData(const YAML::Node& data);
protected:
	std::vector<InputPin*> p_inputs;
	std::vector<OutputPin*> p_outputs;
	bool m_evaluated = false;
private:
	int m_id = -1;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
