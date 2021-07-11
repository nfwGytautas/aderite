#pragma once

#include <vector>
#include <thread>

#include "aderite/core/threading/thread.hpp"
#include "aderite/utility/pointer.hpp"

namespace aderite {
	namespace thread {

		/**
		 * @brief Thread handler for aderite
		*/
		class threader {
		public:
			/**
			 * @brief Initializes the threader
			*/
			bool init();

			std::thread::id get_render_thread_id() const {
				return m_renderThreadID;
			}

		private:
			threader() {}
			friend class engine;
		private:
			std::vector<ref<thread>> m_threads;
			std::thread::id m_renderThreadID;
		};

	}
}
