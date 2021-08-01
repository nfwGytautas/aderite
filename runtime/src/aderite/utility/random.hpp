#pragma once

#include <string>

namespace aderite {
	namespace random {

		/**
		 * @brief Generates a UUID (kinda)
		*/
		std::string generate_uuid();

		/**
		 * @brief Generates a random string of specified length
		*/
		std::string generate_string(const size_t& len);
	}
}
