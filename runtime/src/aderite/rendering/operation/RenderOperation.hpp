#pragma once

#include <bgfx/bgfx.h>
#include "aderite/rendering/Forward.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Operation used to render color and depth to a target
 */
class RenderOperation : public OperationBase {
public:
    RenderOperation();

    // Inherited via OperationBase
    virtual void initialize() override;
    virtual void execute(PipelineState* state) override;
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    /**
     * @brief Execute a single draw call
     * @param dc Draw call to execute
     */
    void executeDrawCall(const DrawCall& dc);

private:
    uint8_t m_viewId = 0xFF;
};

} // namespace rendering
} // namespace aderite
