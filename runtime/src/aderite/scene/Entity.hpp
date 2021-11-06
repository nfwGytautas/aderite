#pragma once

#include <entt/Entity/registry.hpp>

#include "aderite/scene/Forward.hpp"
#include "aderite/scene/Scene.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Wrapper for entt Entity
 */
class Entity {
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene);
    Entity(const Entity& other) = default;

    /**
     * @brief Add component to Entity
     */
    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        T& component = m_scene->getEntityRegistry().emplace<T>(m_handle, std::forward<Args>(args)...);
        m_scene->onComponentAdded<T>(*this, component);
        return component;
    }

    /**
     * @brief Get component
     */
    template<typename T>
    T& getComponent() {
        return m_scene->getEntityRegistry().get<T>(m_handle);
    }

    /**
     * @brief Check if Entity has component
     */
    template<typename T>
    bool hasComponent() {
        return m_scene->getEntityRegistry().try_get<T>(m_handle) != nullptr;
    }

    /**
     * @brief Remove component
     */
    template<typename T>
    void removeComponent() {
        m_scene->onComponentRemoved<T>(*this, getComponent<T>());
        m_scene->getEntityRegistry().remove<T>(m_handle);
    }

    operator bool() const {
        return m_handle != entt::null;
    }
    operator entt::entity() const {
        return m_handle;
    }
    operator uint32_t() const {
        return (uint32_t)m_handle;
    }

    bool operator==(const Entity& other) const {
        return m_handle == other.m_handle && m_scene == other.m_scene;
    }

    bool operator!=(const Entity& other) const {
        return !(*this == other);
    }

    /**
     * @brief Returns the Scene that the Entity belongs to
     */
    Scene* getScene() const {
        return m_scene;
    }

    /**
     * @brief Returns the handle of this entity
     */
    entt::entity getHandle() const {
        return m_handle;
    }

    /**
     * @brief Returns empty Entity
     */
    static Entity null() {
        return Entity(entt::null, nullptr);
    }

private:
    entt::entity m_handle {entt::null};
    Scene* m_scene = nullptr;
};

/**
 * @brief Wrapper for entt Entity
 */
class ConstEntity {
public:
    ConstEntity(entt::entity handle, const Scene* scene);

    /**
     * @brief Get component
     */
    template<typename T>
    const T& getComponent() const {
        return m_scene->getEntityRegistry().get<T>(m_handle);
    }

    /**
     * @brief Check if Entity has component
     */
    template<typename T>
    bool hasComponent() const {
        return m_scene->getEntityRegistry().try_get<T>(m_handle) != nullptr;
    }

    operator bool() const {
        return m_handle != entt::null;
    }
    operator entt::entity() const {
        return m_handle;
    }
    operator uint32_t() const {
        return (uint32_t)m_handle;
    }

    bool operator==(const ConstEntity& other) const {
        return m_handle == other.m_handle && m_scene == other.m_scene;
    }

    bool operator!=(const ConstEntity& other) const {
        return !(*this == other);
    }

    /**
     * @brief Returns the Scene that the Entity belongs to
     */
    const Scene* getScene() const {
        return m_scene;
    }

    /**
     * @brief Returns the handle of this entity
     */
    entt::entity getHandle() const {
        return m_handle;
    }

    /**
     * @brief Returns empty Entity
     */
    static Entity null() {
        return Entity(entt::null, nullptr);
    }

private:
    entt::entity m_handle {entt::null};
    const Scene* m_scene = nullptr;
};

} // namespace scene
} // namespace aderite
