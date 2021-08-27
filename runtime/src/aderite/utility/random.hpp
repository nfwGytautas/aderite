#pragma once

#include <string>
#include "aderite/utility/macros.hpp"

ADERITE_UTILITY_NAMESPACE_BEGIN

/**
 * @brief Generates a UUID (kinda)
*/
std::string generateUuid();

/**
 * @brief Generates a random string of specified length
*/
std::string generateString(const size_t& len);

ADERITE_UTILITY_NAMESPACE_END
