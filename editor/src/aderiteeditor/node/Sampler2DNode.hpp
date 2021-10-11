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
	Sampler2DNode(int id, Graph* graph);

	// Inherited via Node
	virtual void renderUI() override;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
