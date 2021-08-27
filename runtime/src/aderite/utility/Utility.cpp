#include "Utility.hpp"

ADERITE_UTILITY_NAMESPACE_BEGIN

size_t combineHash(const size_t& l, const size_t& r) {
	return l ^ (r << 1);
}

ADERITE_UTILITY_NAMESPACE_END
