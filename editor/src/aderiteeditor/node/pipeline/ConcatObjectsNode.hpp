#pragma once

#include <string>
#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Node for taking n amount of single objects and producing an array output
*/
class ConcatObjectsNode : public Node
{
public:
	ConcatObjectsNode(int id, Graph* graph, const std::string& type);

	/**
	 * @brief Changes the type of the concat node
	 * @param newType New type of the node
	*/
	void setType(const std::string& newType);

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;
	virtual bool onConnectToInput(InputPin* target, OutputPin* source) override;
private:
	std::string m_currentType;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
