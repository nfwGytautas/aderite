#include "threader.hpp"

namespace aderite {
	namespace thread {

		bool threader::init() {
			m_renderThreadID = std::this_thread::get_id();

			return true;
		}

		void threader::sleep_caller(size_t ms) {
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		}

	}
}
