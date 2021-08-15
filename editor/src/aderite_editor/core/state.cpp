#include "state.hpp"

#include "aderite_editor/core/event_router.hpp"
#include "aderite_editor/core/project.hpp"

namespace aderite {
	namespace editor {

		ievent_sink* state::Sink = nullptr;
		project* state::Project = nullptr;

	}
}
