#pragma once

#include "aderiteeditor/node/Node.hpp"

namespace aderite {
namespace node {

/**
 * @brief Node that executes a rendering pass
 */
class RenderNode : public Node {
public:
    RenderNode();

    /**
     * @brief Change the node to array type
     */
    void convertToArray();

    /**
     * @brief Change the node to single object type
     */
    void convertToSingle();

    // Inherited via Node
    virtual const char* getNodeName() const override;
    virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
    virtual bool onConnectToInput(InputPin* target, OutputPin* source) override;
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    bool m_array = false;
};

} // namespace node
} // namespace aderite
