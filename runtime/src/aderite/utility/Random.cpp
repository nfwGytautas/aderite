#include "Random.hpp"
#include <random>

namespace aderite {
namespace utility {

static std::random_device dev;
static std::mt19937 rng(dev());

std::string generateUuid() {
    std::uniform_int_distribution<int> dist(0, 15);

    const char* v = "0123456789abcdef";
    const bool dash[] = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};

    std::string res;
    for (int i = 0; i < 16; i++) {
        if (dash[i])
            res += "-";
        res += v[dist(rng)];
        res += v[dist(rng)];
    }
    return res;
}

std::string generateString(const size_t& len) {
    std::uniform_int_distribution<int> dist(0, 61);

    const char* v = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

    std::string res = "a";
    for (int i = 0; i < len - 1; i++) {
        res += v[dist(rng)];
    }
    return res;
}

} // namespace utility
} // namespace aderite
