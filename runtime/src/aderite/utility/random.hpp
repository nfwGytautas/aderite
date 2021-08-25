#pragma once

#include <string>

namespace aderite {
	namespace random {

		/**
		 * @brief Generates a UUID (kinda)
		*/
		std::string generateUuid();

		/**
		 * @brief Generates a random string of specified length
		*/
		std::string generateString(const size_t& len);
	}
}
