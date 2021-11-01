#pragma once

#include <string>

#include <entt/entt.hpp>

namespace aderite {
namespace scene {

/**
 * @brief Component containing all MetaComponent information about an Entity, such as it's name, tags, etc.
 */
struct MetaComponent {
    std::string Name;
    entt::entity This;

    MetaComponent() = default;
    MetaComponent(const MetaComponent&) = default;
    MetaComponent(const std::string& name) : Name(name) {}
};

} // namespace scene
} // namespace aderite