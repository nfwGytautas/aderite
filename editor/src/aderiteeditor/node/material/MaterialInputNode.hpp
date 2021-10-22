#pragma once

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/node/Node.hpp"
#include "aderiteeditor/asset/Forward.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Input node for material shaders
*/
class MaterialInputNode : public Node 
{
public:
	MaterialInputNode();

	/**
	 * @brief Sets the type of the input node
	 * @param type New type
	*/
	void setType(asset::EditorMaterialType* type);

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void renderBody() override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;

private:
	void generatePins();

private:
	asset::EditorMaterialType* m_material = nullptr;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
