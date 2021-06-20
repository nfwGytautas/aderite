#include "thread.hpp"

namespace aderite {
	namespace thread {
		thread::thread() {
			m_thread = std::thread(std::bind(&thread::start, this));
		}

		thread::~thread() {
			this->join();
		}

		void thread::join() {
			m_wants_to_join = true;
		}

		void thread::invoke(ref<thread_invoke_base> i) {
			std::unique_lock<std::mutex> latch(m_mutex);
			m_invoke_queue.push(i);
		}

		void thread::process_n_invokes(size_t count) {
			std::unique_lock<std::mutex> latch(m_mutex);
			if (count == 0) {
				count = m_invoke_queue.size();
			}

			for (size_t i = 0; i < count; i++) {
				ref<thread_invoke_base> tib = m_invoke_queue.front();
				m_invoke_queue.pop();
				tib->call();
				tib->wait();
			}
		}
	}
}
