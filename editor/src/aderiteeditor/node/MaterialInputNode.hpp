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
	MaterialInputNode(int id, Graph* graph, asset::MaterialTypeAsset* material);

	// Inherited via Node
	virtual void renderUI() override;
private:
	asset::MaterialTypeAsset* m_material = nullptr;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
