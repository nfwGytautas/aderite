#pragma once

#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Node that executes a rendering pass
*/
class RenderNode : public Node
{
public:
	RenderNode();

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
	virtual bool onConnectToInput(InputPin* target, OutputPin* source) override;
	virtual io::SerializableType getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
private:
	bool m_array = false;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
