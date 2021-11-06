#pragma once

#include <string>

#include "aderite/rendering/operation/OperationBase.hpp"

#include "aderiteeditor/node/Node.hpp"

namespace aderite {
namespace node {

/**
 * @brief Node for converting one input type to another
 */
class ConvertNode : public Node {
public:
    ConvertNode();

    /**
     * @brief Change the type of the convert node
     */
    void setFromType(const std::string& from);

    /**
     * @brief Change the type of the convert node
     */
    void setToType(const std::string& to);

    // Inherited via Node
    virtual const char* getNodeName() const override;
    virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
    virtual bool onConnectToInput(InputPin* target, OutputPin* source) override;
    virtual bool onConnectToOutput(OutputPin* target, InputPin* source) override;
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    void handlePipelineConvert(compiler::PipelineEvaluator* pe);
    void handleShaderConvert(compiler::ShaderEvaluator* se);

    // Conversion functions
    using PipelineConversionFn = rendering::OperationBase* (ConvertNode::*)(rendering::OperationBase*);
    void convert(compiler::PipelineEvaluator* pe, PipelineConversionFn cfn);
    void applyArray(compiler::PipelineEvaluator* pe, PipelineConversionFn cfn);

    // Conversion methods
};

} // namespace node
} // namespace aderite
