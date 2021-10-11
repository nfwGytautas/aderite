#include "Sampler2DNode.hpp"

#include "MaterialInputNode.hpp"

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

Sampler2DNode::Sampler2DNode(int id, Graph* graph)
    : Node(id)
{
    // Create pins
    p_inputs.push_back(
        graph->createInputPin(
            asset::prop::getNameForType(
                asset::prop::PropertyType::TEXTURE_2D), 
                "Texture"
        )
    );

    p_outputs.push_back(
        graph->createOutputPin(
            asset::prop::getNameForType(
                asset::prop::PropertyType::VEC4), 
            "Color"
        )
    );
}

void Sampler2DNode::renderUI() {
    ImNodes::BeginNode(getId());

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Sampler 2D");
    ImNodes::EndNodeTitleBar();

    renderPins();

    ImNodes::EndNode();
}

ADERITE_EDITOR_NODE_NAMESPACE_END
