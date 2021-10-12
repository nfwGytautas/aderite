#pragma once

#include "aderite/interfaces/ISerializable.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/node/Forward.hpp"
#include "aderiteeditor/compiler/Forward.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Base node class
 * NOTES: Inherited nodes should define their constructor as (id, Graph*, args...)
*/
class Node : public interfaces::ISerializable
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
	void renderUI();

	/**
	 * @brief Evaluate this node data and store information in it's output pins
	 * @param writer Writer
	*/
	virtual void evaluate(compiler::ShaderWriter* writer);

	/**
	 * @brief Evaluates dependencies for this node
	 * @param writer Writer
	*/
	void evaluateDependencies(compiler::ShaderWriter* writer);

	/**
	 * @brief Resets evaluate flag on node
	*/
	void resetEvaluateFlag();
protected:
	/**
	 * @brief Returns the name of the node
	 * @return Name of the node
	*/
	virtual const char* getNodeName() const = 0;

	/**
	 * @brief Optional UI body
	*/
	virtual void renderBody() {};

	/**
	 * @brief Serializes pins and position
	*/
	void serializeData(YAML::Emitter& out);

	/**
	 * @brief Deserializes pins and position
	*/
	void deserializeData(YAML::Node& data);
private:
	/**
	 * @brief Renders input and output pins
	*/
	void renderPins();
protected:
	std::vector<InputPin*> p_inputs;
	std::vector<OutputPin*> p_outputs;
	bool m_evaluated = false;
private:
	int m_id = -1;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
