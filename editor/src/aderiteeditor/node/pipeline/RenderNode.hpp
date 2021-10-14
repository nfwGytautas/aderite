#pragma once

#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Node that executes a rendering pass
*/
class RenderNode : public Node
{
public:
	RenderNode(int id, Graph* graph);

	/**
	 * @brief Change the node to array type
	*/
	void convertToArray();

	/**
	 * @brief Change the node to single object type
	*/
	void convertToSingle();

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;
	virtual bool onConnectToInput(InputPin* target, OutputPin* source) override;
private:
	bool m_array = false;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
