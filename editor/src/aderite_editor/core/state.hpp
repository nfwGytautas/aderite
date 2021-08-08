#pragma once

#include "aderite_editor/core/event_router.hpp"
#include "aderite_editor/core/project.hpp"

namespace aderite {
	namespace editor {

		/**
		 * @brief Global(makes life so much easier in this case) state of the editor
		*/
		class state {
		public:
			static ievent_sink* Sink;
			static project* Project;
		};

	}
}