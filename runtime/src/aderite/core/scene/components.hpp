#pragma once

/**
 * @brief All components supported natively by aderite are described in this file
*/

#include <string>

namespace aderite {
	namespace scene {
		namespace components {

			/**
			 * @brief Component containing all meta information about an entity, such as it's name, tags, etc.
			*/
			struct meta {
				std::string Name;

				meta() = default;
				meta(const meta&) = default;
				meta(const std::string& name)
					: Name(name) {}
			};

		}
	}
}
