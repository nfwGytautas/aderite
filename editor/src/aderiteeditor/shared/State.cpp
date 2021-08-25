#include "State.hpp"

#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/project.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

IEventSink* shared::State::Sink = nullptr;
Project* shared::State::Project = nullptr;

ADERITE_EDITOR_SHARED_NAMESPACE_END
