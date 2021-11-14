#pragma once

#include "aderite/rendering/Forward.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Operation used to render color and depth to a target
 */
class RenderOperation final : public OperationBase {
public:
    RenderOperation();

    // Inherited via OperationBase
    void initialize() override;
    void execute(PipelineState* state) override;
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    /**
     * @brief Execute a single draw call
     * @param dc Draw call to execute
     */
    void executeDrawCall(const DrawCall& dc) const;

private:
    uint8_t m_viewId = 0xFF;
};

} // namespace rendering
} // namespace aderite
