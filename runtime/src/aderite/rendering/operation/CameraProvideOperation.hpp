#pragma once

#include <glm/glm.hpp>

#include "aderite/rendering/operation/Forward.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/scene/components/Components.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Operation used to create an entity camera provider
 */
class CameraProvideOperation : public OperationBase {
public:
    virtual ~CameraProvideOperation() {};

    /**
     * @brief Figure out the main camera and set it
     */
    virtual void execute(PipelineState* state) override;

    // Inherited via OperationBase
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projMatrix;
};

} // namespace rendering
} // namespace aderite
