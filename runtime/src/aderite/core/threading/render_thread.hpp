#pragma once

#include <functional>
#include "aderite/core/threading/thread.hpp"

namespace aderite {
	namespace thread {

		/**
		 * @brief Creates a rendering thread, it usually is the main thread, that means that no new thread will be created
		 * but this allows to fit the rendering thread into the system
		*/
		class render_thread : public thread {
		public:
			/**
			 * @brief Creates a rendering thread
			*/
			render_thread();

			~render_thread();

			// Inherited via thread
			virtual void start() override;
		};

	}
}
