#include "State.hpp"

#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/project.hpp"

namespace aderite {
namespace editor {

editor::IEventSink* State::Sink = nullptr;
editor::Project* State::Project = nullptr;
editor::EditorCamera* State::EditorCamera = nullptr;
bgfx::FrameBufferHandle State::DebugRenderHandle = BGFX_INVALID_HANDLE;
bool State::IsGameMode = false;

io::SerializableObject* State::getSelectedObject() const {
    return m_selectedObject;
}

void State::setSelectedObject(io::SerializableObject* object) {
    m_selectedObject = object;
}

State& State::getInstance() {
    static State state;
    return state;
}

} // namespace editor
} // namespace aderite
