#include "window.hpp"

#include "aderite/aderite.hpp"

namespace aderite {

	void window::make_active() {
		engine::get()->get_window_manager()->active_window_changed(this);
	}

}
