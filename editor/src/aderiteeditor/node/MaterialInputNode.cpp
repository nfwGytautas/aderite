#include "MaterialInputNode.hpp"

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

MaterialInputNode::MaterialInputNode(int id, Graph* graph, asset::MaterialTypeAsset* material)
	: Node(id), m_material(material)
{
	// Create pins
    for (int i = 0; i < m_material->getFields().Properties.size(); i++) {
        asset::prop::Property* prop = m_material->getFields().Properties[i];
        p_outputs.push_back(
            graph->createOutputPin(asset::prop::getNameForType(prop->getType()), prop->getName())
        );
    }
}

void MaterialInputNode::renderUI() {
    ImNodes::BeginNode(getId());

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Material input");
    ImNodes::EndNodeTitleBar();

    ImGui::Text(m_material->getName().c_str());
    ImGui::Spacing();

    renderPins();

    ImNodes::EndNode();
}

ADERITE_EDITOR_NODE_NAMESPACE_END
