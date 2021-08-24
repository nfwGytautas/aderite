#pragma once

#include <entt/Entity/registry.hpp>
#include "aderite/utility/macros.hpp"
#include "aderite/scene/Forward.hpp"

ADERITE_SCENE_NAMESPACE_BEGIN

/**
 * @brief Wrapper for entt Entity
*/
class Entity
{
public:
	Entity() = default;
	Entity(entt::entity handle, Scene* scene);
	Entity(const Entity& other) = default;

	/**
	 * @brief Add component to Entity
	*/
	template<typename T, typename... Args>
	T& add_component(Args&&... args) {
		T& component = m_scene->get_entity_registry().emplace<T>(m_handle, std::forward<Args>(args)...);
		return component;
	}

	/**
	 * @brief Get component
	*/
	template<typename T>
	T& get_component() {
		return m_scene->get_entity_registry().get<T>(m_handle);
	}

	/**
	 * @brief Check if Entity has component
	*/
	template<typename T>
	bool has_component() {
		return m_scene->get_entity_registry().try_get<T>(m_handle) != nullptr;
	}

	/**
	 * @brief Remove component
	*/
	template<typename T>
	void remove_component() {
		m_scene->get_entity_registry().remove<T>(m_handle);
	}

	operator bool() const { return m_handle != entt::null; }
	operator entt::entity() const { return m_handle; }
	operator uint32_t() const { return (uint32_t)m_handle; }

	bool operator==(const Entity& other) const {
		return m_handle == other.m_handle && m_scene == other.m_scene;
	}

	bool operator!=(const Entity& other) const {
		return !(*this == other);
	}

	/**
	 * @brief Returns the Scene that the Entity belongs to
	*/
	Scene* get_scene() const {
		return m_scene;
	}

	/**
	 * @brief Returns empty Entity
	*/
	static Entity null() {
		return Entity(entt::null, nullptr);
	}
private:
	entt::entity m_handle { entt::null };
	Scene* m_scene = nullptr;
};

ADERITE_SCENE_NAMESPACE_END
