#pragma once

#include <glm/glm.hpp>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"

namespace aderite {
namespace asset {

/**
 * @brief An asset class used to represent a list of colliders and triggers that can be assigned to entities
 */
class ColliderListAsset final : public io::SerializableObject {
public:
    ~ColliderListAsset();

    /**
     * @brief Attach colliders in the list to the specified actor
     * @param actor Actor to attach to
     * @param globalScale Scale vector to be applied
     */
    void attachTo(physics::PhysicsActor* actor, const glm::vec3& globalScale);

    /**
     * @brief Add an already created collider, this collider is managed by the collider list
     * @param collider Collider to add
     */
    void add(physics::Collider* collider);

    /**
     * @brief Removes collider from list
     * @param collider Collider to remove
     */
    void remove(physics::Collider* collider);

    /**
     * @brief Gets the collider iteration used only when changing state
     */
    size_t getIteration() const;

    /**
     * @brief Increments the iteration of the list
     */
    void incrementIteration();

    physics::Collider* get(size_t idx) const {
        return m_colliders[idx];
    }

    physics::Collider* operator[](size_t idx) const {
        return m_colliders[idx];
    }

    size_t size() const {
        return m_colliders.size();
    }

    auto begin() const {
        return m_colliders.begin();
    }

    auto end() const {
        return m_colliders.end();
    }

    // Inherited via SerializableObject
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    size_t m_iteration = 1;
    std::vector<physics::Collider*> m_colliders;
};

} // namespace asset
} // namespace aderite
