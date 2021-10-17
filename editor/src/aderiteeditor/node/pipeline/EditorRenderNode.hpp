#pragma once

#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Node used to set at which point will the engine will render colliders, gizmos, etc.
*/
class EditorRenderNode : public Node
{
public:
	EditorRenderNode();

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual io::SerializableType getType() override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
