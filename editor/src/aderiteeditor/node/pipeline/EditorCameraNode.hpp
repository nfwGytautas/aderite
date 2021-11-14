#pragma once

#include "aderiteeditor/node/Node.hpp"

namespace aderite {
namespace node {

/**
 * @brief Node used to provide the editor camera
 */
class EditorCameraNode : public Node {
public:
    EditorCameraNode();

    // Inherited via Node
    virtual const char* getNodeName() const override;
    virtual void evaluate(compiler::GraphEvaluator* evaluator) override;
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

} // namespace node
} // namespace aderite
