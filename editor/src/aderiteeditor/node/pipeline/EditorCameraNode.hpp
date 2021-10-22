#pragma once

#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Node used to provide the editor camera
*/
class EditorCameraNode : public Node
{
public:
	EditorCameraNode();

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
