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
	SelectObjectNode();

	/**
	 * @brief Changes the type of the concat node
	 * @param newType New type of the node
	*/
	void setType(const std::string& newType);

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual bool onConnectToInput(InputPin* target, OutputPin* source) override;
	virtual void renderBody() override;
	virtual io::SerializableType getType() override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
private:
	std::string m_currentType;
	size_t m_index = 0;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
