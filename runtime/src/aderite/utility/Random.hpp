#pragma once

#include <string>

#include "aderite/utility/macros.hpp"

namespace aderite {
namespace utility {

/**
 * @brief Generates a UUID (kinda)
 */
std::string generateUuid();

/**
 * @brief Generates a random string of specified length
 */
std::string generateString(const size_t& len);

} // namespace utility
} // namespace aderite
