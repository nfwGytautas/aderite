#pragma once

#include <string>
#include "aderiteeditor/node/Node.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

/**
 * @brief Node for converting one input type to another
*/
class ConvertNode : public Node
{
public:
	ConvertNode(int id, Graph* graph, const std::string& from, const std::string& to);

	// Inherited via Node
	virtual const char* getNodeName() const override;
	virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

private:
	void handlePipelineConvert(compiler::PipelineEvaluator* pe);
	void handleShaderConvert(compiler::ShaderEvaluator* se);
private:
	std::string m_from;
	std::string m_to;
};

ADERITE_EDITOR_NODE_NAMESPACE_END
