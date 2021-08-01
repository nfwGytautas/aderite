#pragma once

#include <entt/entity/registry.hpp>
#include "aderite/core/scene/scene.hpp"

namespace aderite {
	namespace scene {

		/**
		 * @brief Wrapper for entt entity
		*/
		class entity
		{
		public:
			entity() = default;
			entity(entt::entity handle, scene* scene);
			entity(const entity& other) = default;

			/**
			 * @brief Add component to entity
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
			 * @brief Check if entity has component
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

			bool operator==(const entity& other) const
			{
				return m_handle == other.m_handle && m_scene == other.m_scene;
			}

			bool operator!=(const entity& other) const
			{
				return !(*this == other);
			}

			/**
			 * @brief Returns empty entity
			*/
			static entity null() {
				return entity(entt::null, nullptr);
			}
		private:
			entt::entity m_handle { entt::null };
			scene* m_scene = nullptr;
		};

	}
}
