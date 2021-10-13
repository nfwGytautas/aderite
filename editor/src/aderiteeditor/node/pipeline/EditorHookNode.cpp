#include "EditorHookNode.hpp"

#include "aderite/asset/property/Property.hpp"
#include "aderite/rendering/operation/DebugOperation.hpp"
#include "aderite/rendering/operation/CameraProvideOperation.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderiteeditor/node/Graph.hpp"
#include "aderiteeditor/node/InputPin.hpp"
#include "aderiteeditor/node/OutputPin.hpp"
#include "aderiteeditor/node/pipeline/Properties.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

EditorHookNode::EditorHookNode(int id, Graph* graph)
    : Node(id)
{
    // Output
    p_outputs.push_back(graph->createOutputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Camera),
        "Camera"
    ));

    p_outputs.push_back(graph->createOutputPin(
        this,
        pipeline::getTypeName(pipeline::PropertyType::Target),
        "Target"
    ));
}

const char* EditorHookNode::getNodeName() const {
    return "Editor hook";
}

void EditorHookNode::evaluate(compiler::GraphEvaluator* evaluator) {
    evaluateDependencies(evaluator);
    compiler::PipelineEvaluator* pe = static_cast<compiler::PipelineEvaluator*>(evaluator);
    rendering::DebugOperation* op = new rendering::DebugOperation();
    p_outputs[0]->setValue(pe->addOperation(op->getCameraProvider()));
    p_outputs[1]->setValue(pe->addOperation(op->getTargetProvider()));
    pe->addOperation(op);
    m_evaluated = true;
}

bool EditorHookNode::serialize(YAML::Emitter& out) {
    out << YAML::BeginMap;
    out << YAML::Key << "NodeType" << YAML::Value << "EditorHook";
    serializeData(out);
    out << YAML::EndMap;
    return false;
}

bool EditorHookNode::deserialize(YAML::Node& data) {
    deserializeData(data);
    return true;
}

ADERITE_EDITOR_NODE_NAMESPACE_END
