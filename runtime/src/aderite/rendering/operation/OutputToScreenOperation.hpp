#pragma once

#include "aderite/rendering/operation/OperationBase.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Operation used to output a target to screen
 */
class OutputToScreenOperation final : public OperationBase {
public:
    OutputToScreenOperation();

    // Inherited via OperationBase
    void execute(PipelineState* state) override;
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

} // namespace rendering
} // namespace aderite
