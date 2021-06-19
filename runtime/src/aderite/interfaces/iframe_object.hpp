#pragma once

namespace aderite {
	namespace interfaces {

		/**
		 * @brief Class used to provide begin_frame, end_frame and ordering
		*/
		class iframe_object {
		public:
			virtual ~iframe_object() {}

			/**
			 * @brief Start of frame
			*/
			virtual void begin_frame() = 0;

			/**
			 * @brief End of frame
			*/
			virtual void end_frame() = 0;
		};

	}
}
