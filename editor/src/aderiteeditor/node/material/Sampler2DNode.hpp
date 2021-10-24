#pragma once

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Input node for sampler 2D
*/
class Sampler2DNode : public Node
{
public:
	Sampler2DNode();

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
