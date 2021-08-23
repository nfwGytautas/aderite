#pragma once

namespace aderite {
	class renderer;

	namespace rendering {
		class draw_call;
	}

	namespace interfaces {

		/**
		 * @brief Generic renderable item interface
		*/
		class irenderable {
		public:
			virtual ~irenderable() {}

			/**
			 * @brief Fills draw call object information from the object state
			 * @param dc Draw call instance
			*/
			virtual void fill_draw_call(rendering::draw_call* dc) = 0;
		};

	}
}