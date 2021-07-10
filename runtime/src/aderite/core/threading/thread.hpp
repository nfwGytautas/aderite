#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <condition_variable>

#include "aderite/utility/pointer.hpp"
#include "aderite/core/threading/thread_invoke.hpp"

namespace aderite {
	namespace thread {

		/**
		 * @brief Class used to provide a sync mechanism to threads
		*/
		class sync {
		public:

		private:
			std::condition_variable m_cv;
		};

		/**
		 * @brief Base class for all threader handled threads
		*/
		class thread {
		public:
			thread();
			virtual ~thread();

			/**
			 * @brief Starts the thread
			*/
			virtual void start() = 0;

			/**
			 * @brief Joins the thread to the caller
			*/
			void join();

			/**
			 * @brief Invokes the specified function on the thread
			*/
			void invoke(ref<thread_invoke_base> i);

			/**
			 * @brief Processes the specified amount of invokes and returns
			 * @param count Number of invokes to wait if this is 0 then all invokes are processed
			*/
			void process_n_invokes(size_t count);

			/**
			 * @brief Returns the thread id of this thread object
			*/
			std::thread::id id() {
				return m_thread.get_id();
			}
		protected:
			bool m_wants_to_join = false;
			std::thread m_thread;
			std::mutex m_mutex;
			std::queue<ref<thread_invoke_base>> m_invoke_queue;
		};
	}
}
