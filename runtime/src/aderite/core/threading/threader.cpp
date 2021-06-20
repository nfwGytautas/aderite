#include "threader.hpp"

namespace aderite {
	namespace thread {

		bool threader::init() {
			m_render = new render_thread();
			return true;
		}

		relay_ptr<render_thread> threader::get_render_thread() {
			return m_render.relay();
		}

	}
}
