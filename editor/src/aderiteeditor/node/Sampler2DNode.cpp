#include "Sampler2DNode.hpp"

#include "MaterialInputNode.hpp"

#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/windows/backend/node/imnodes.h"
#include "aderiteeditor/compiler/ShaderWriter.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

Sampler2DNode::Sampler2DNode(int id, Graph* graph)
    : Node(id)
{
    // Create pins
    p_inputs.push_back(
        graph->createInputPin(
            this,
            asset::prop::getNameForType(
                asset::prop::PropertyType::TEXTURE_2D), 
                "Texture"
        )
    );

    p_outputs.push_back(
        graph->createOutputPin(
            this,
            asset::prop::getNameForType(
                asset::prop::PropertyType::VEC4), 
            "Color"
        )
    );
}

const char* Sampler2DNode::getNodeName() const {
    return "Sampler 2D";
}

void Sampler2DNode::evaluate(compiler::ShaderWriter* writer) {
    evaluateDependencies(writer);
    compiler::Variable var = writer->add2DSamplingInstruction(p_inputs[0]->getValue());
    p_outputs[0]->setValue(var);
    m_evaluated = true;
}

bool Sampler2DNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "Sampler2D";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool Sampler2DNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END

