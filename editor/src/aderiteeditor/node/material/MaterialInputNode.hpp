#pragma once

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Input node for material shaders
*/
class MaterialInputNode : public Node 
{
public:
	MaterialInputNode();

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void renderBody() override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual io::SerializableType getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;
private:
	asset::MaterialTypeAsset* m_material = nullptr;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
