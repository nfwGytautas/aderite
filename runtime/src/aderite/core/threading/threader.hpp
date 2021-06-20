#pragma once

#include <vector>
#include <thread>

#include "aderite/core/threading/thread.hpp"
#include "aderite/utility/pointer.hpp"

#include "aderite/core/threading/render_thread.hpp"

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

			/**
			 * @brief Returns the rendering thread
			*/
			relay_ptr<render_thread> get_render_thread();
		private:
			std::vector<ref<thread>> m_threads;

			// Long running threads
			ref<render_thread> m_render = nullptr;
		};

	}
}
