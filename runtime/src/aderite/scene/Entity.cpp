#include "Entity.hpp"

ADERITE_SCENE_NAMESPACE_BEGIN

Entity::Entity(entt::entity handle, Scene* scene)
	: m_handle(handle), m_scene(scene)
{}

ADERITE_SCENE_NAMESPACE_END
