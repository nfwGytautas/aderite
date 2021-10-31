#include "Utility.hpp"

namespace aderite {
namespace utility {

size_t combineHash(const size_t& l, const size_t& r) {
    return l ^ (r << 1);
}

} // namespace utility
} // namespace aderite
