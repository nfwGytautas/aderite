#pragma once

#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Output node for material shaders
*/
class MaterialOutputNode : public Node
{
public:
	MaterialOutputNode(int id, Graph* graph);

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void evaluate(compiler::ShaderWriter* writer) override;
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
