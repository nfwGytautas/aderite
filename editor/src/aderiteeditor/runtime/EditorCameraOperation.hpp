#pragma once

#include "aderite/rendering/Forward.hpp"
#include "aderite/rendering/operation/Forward.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Operation used to provide an editor camera
 */
class EditorCameraOperation : public rendering::OperationBase {
public:
    // Inherited via EyeProvideOperation
    virtual void execute(rendering::PipelineState* state) override;
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

} // namespace editor
} // namespace aderite
