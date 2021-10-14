#pragma once

#include <string>
#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Node for taking a single item from an array
*/
class SelectObjectNode : public Node
{
public:
	SelectObjectNode(int id, Graph* graph, const std::string& type);

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
	virtual void renderBody() override;
private:
	std::string m_currentType;
	size_t m_index = 0;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
