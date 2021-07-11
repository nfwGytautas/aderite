#include "threader.hpp"

namespace aderite {
	namespace thread {

		bool threader::init() {
			m_renderThreadID = std::this_thread::get_id();

			return true;
		}

	}
}
