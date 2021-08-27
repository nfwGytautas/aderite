#include "State.hpp"

#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/project.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

IEventSink* shared::State::Sink = nullptr;
Project* shared::State::Project = nullptr;
EditorCamera* shared::State::EditorCamera = nullptr;
bool shared::State::IsGameMode = false;

ADERITE_EDITOR_SHARED_NAMESPACE_END
