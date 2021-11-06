#pragma once

#include "aderite/asset/MaterialTypeAsset.hpp"

#include "aderiteeditor/node/Node.hpp"

namespace aderite {
namespace node {

/**
 * @brief Add node for 2 floats, vec2, vec3, vec4
 */
class AddNode : public Node {
public:
    AddNode();

    /**
     * @brief Change the node type
     * @param type New type
     */
    void setType(std::string type);

    // Inherited via Node
    virtual const char* getNodeName() const override;
    virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
    virtual bool onConnectToInput(InputPin* target, OutputPin* source) override;
    virtual bool onConnectToOutput(OutputPin* target, InputPin* source) override;

private:
    std::string m_type;
};

} // namespace node
} // namespace aderite
