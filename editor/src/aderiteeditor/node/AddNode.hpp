#pragma once

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Add node for 2 floats, vec2, vec3, vec4
*/
class AddNode : public Node
{
public:
	AddNode(int id, Graph* graph, asset::prop::PropertyType type = asset::prop::PropertyType::NONE);

	/**
	 * @brief Change the node type
	 * @param type New type
	*/
	void setType(asset::prop::PropertyType type);

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void renderBody() override;
	virtual void evaluate(compiler::ShaderWriter* writer) override;
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;
private:
	void renderSelection(asset::prop::PropertyType type);
private:
	asset::prop::PropertyType m_type;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
