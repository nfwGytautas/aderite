#include "thread_invoke.hpp"

namespace aderite {
	namespace thread {

		void thread_invoke_base::wait() {
			std::unique_lock<std::mutex> latch(m_mutex);
			m_done.wait(latch, [this]() { return m_finished; });
		}

	}
}
