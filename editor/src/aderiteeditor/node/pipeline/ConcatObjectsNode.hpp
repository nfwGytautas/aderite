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
	ConcatObjectsNode();

	/**
	 * @brief Changes the type of the concat node
	 * @param newType New type of the node
	*/
	void setType(const std::string& newType);

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual bool onConnectToInput(InputPin* target, OutputPin* source) override;
	virtual io::SerializableType getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
private:
	std::string m_currentType;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
