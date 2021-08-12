#pragma once

namespace aderite {
	namespace editor {
		class ievent_sink;
		class project;

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