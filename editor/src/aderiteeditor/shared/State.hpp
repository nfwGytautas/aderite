#pragma once

#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/Forward.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

/**
 * @brief Global(makes life so much easier in this case) State of the editor
*/
class State {
public:
	static IEventSink* Sink;
	static Project* Project;
};

ADERITE_EDITOR_SHARED_NAMESPACE_END
