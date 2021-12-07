#include "EditorTargetOperation.hpp"

#include "aderite/rendering/PipelineState.hpp"

#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/shared/EditorCamera.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace editor {

EditorTargetOperation::EditorTargetOperation() {}

void EditorTargetOperation::initialize() {
    TargetProvideOperation::initialize();
    editor::State::DebugRenderHandle = getHandle();
}

reflection::Type EditorTargetOperation::getType() const {
    return static_cast<reflection::Type>(reflection::EditorTypes::EditorTargetOp);
}

} // namespace editor
} // namespace aderite
