#pragma once

#include "aderite/asset/MaterialTypeAsset.hpp"

#include "aderiteeditor/node/Node.hpp"

namespace aderite {
namespace node {

/**
 * @brief Input node for sampler 2D
 */
class Sampler2DNode : public Node {
public:
    Sampler2DNode();

    // Inherited via Node
    virtual const char* getNodeName() const override;
    virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

} // namespace node
} // namespace aderite
