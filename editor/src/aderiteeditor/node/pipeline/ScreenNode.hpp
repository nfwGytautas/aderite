#pragma once

#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Output node for render pipeline 
*/
class ScreenNode : public Node
{
public:
	ScreenNode(int id, Graph* graph);

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
