#include "Entity.hpp"

namespace aderite {
namespace scene {

Entity::Entity(entt::entity handle, Scene* scene) : m_handle(handle), m_scene(scene) {}

ConstEntity::ConstEntity(entt::entity handle, const Scene* scene) : m_handle(handle), m_scene(scene) {}

} // namespace scene
} // namespace aderite
