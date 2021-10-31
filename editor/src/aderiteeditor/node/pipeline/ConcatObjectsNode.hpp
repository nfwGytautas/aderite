#pragma once

#include <string>

#include "aderiteeditor/node/Node.hpp"

namespace aderite {
namespace node {

/**
 * @brief Node for taking n amount of single objects and producing an array output
 */
class ConcatObjectsNode : public Node {
public:
    ConcatObjectsNode();

    /**
     * @brief Changes the type of the concat node
     * @param newType New type of the node
     */
    void setType(const std::string& newType);

    // Inherited via Node
    virtual const char* getNodeName() const override;
    virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
    virtual bool onConnectToInput(InputPin* target, OutputPin* source) override;
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    std::string m_currentType;
};

} // namespace node
} // namespace aderite
