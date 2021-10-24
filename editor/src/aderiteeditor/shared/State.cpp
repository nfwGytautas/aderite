#include "State.hpp"

#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/project.hpp"

namespace aderite {
namespace editor {

shared::IEventSink* State::Sink = nullptr;
shared::Project* State::Project = nullptr;
shared::EditorCamera* State::EditorCamera = nullptr;
bgfx::FrameBufferHandle State::DebugRenderHandle = BGFX_INVALID_HANDLE;
SelectableObject State::LastSelectedObject = {};
bool State::IsGameMode = false;

}
}
