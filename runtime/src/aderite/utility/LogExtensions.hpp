#pragma once

#include <glm/glm.hpp>
#include <spdlog/fmt/bundled/format.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/utility/Log.hpp"

/**
 * @brief Formatter for glm::vec3
 */
template<>
struct fmt::formatter<glm::vec3> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const glm::vec3& input, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "(x={}, y={}, z={})", input.x, input.y, input.z);
    }
};

/**
 * @brief Formatter for glm::quat
 */
template<>
struct fmt::formatter<glm::quat> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const glm::quat& input, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "(w={}, x={}, y={}, z={})", input.w, input.x, input.y, input.z);
    }
};

/**
 * @brief Helper class for creating a log block
 */
class LogBlock {
public:
    LogBlock() {
        LOG_TRACE("");
    }

    ~LogBlock() {
        //LOG_TRACE("");
    }
};

#define ADERITE_LOG_BLOCK LogBlock log_block
