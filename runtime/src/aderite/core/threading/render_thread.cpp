#include "render_thread.hpp"

#include "aderite/core/aderite.hpp"
#include "aderite/utility/log.hpp"

namespace aderite {
	namespace thread {

		render_thread::render_thread()
			: thread()
		{}

		render_thread::~render_thread() {

		}

		void render_thread::start() {
			// Didn't create thread
			LOG_DEBUG("Creating rendering thread");

			while (!m_wants_to_join) {
				// If there is time process calls
				this->process_n_invokes(0);


				engine::get()->begin_frame();
				// START OF RENDER CODE

				engine::get()->get_renderer()->render();

				// END OF RENDER CODE
				engine::get()->end_frame();
			}
		}
	}
}
