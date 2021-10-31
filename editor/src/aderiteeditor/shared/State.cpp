#include "State.hpp"

#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/project.hpp"

namespace aderite {
namespace editor {

editor::IEventSink* State::Sink = nullptr;
editor::Project* State::Project = nullptr;
editor::EditorCamera* State::EditorCamera = nullptr;
bgfx::FrameBufferHandle State::DebugRenderHandle = BGFX_INVALID_HANDLE;
SelectableObject State::LastSelectedObject = {};
bool State::IsGameMode = false;

} // namespace editor
} // namespace aderite
