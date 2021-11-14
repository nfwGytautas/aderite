#pragma once

#include <glm/glm.hpp>

#include "aderite/rendering/operation/OperationBase.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Operation used to create an entity camera provider
 */
class CameraProvideOperation final : public OperationBase {
public:
    ~CameraProvideOperation() {}

    /**
     * @brief Figure out the main camera and set it
     */
    void execute(PipelineState* state) override;

    // Inherited via OperationBase
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projMatrix;
};

} // namespace rendering
} // namespace aderite
