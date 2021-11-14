#pragma once

#include "aderite/physics/Collider.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Simple box collider
 */
class BoxCollider final : public Collider {
public:
    /**
     * @brief Returns the size of the box collider
     * @return Box collider size
     */
    glm::vec3 getSize() const;

    /**
     * @brief Sets the size of the box collider
     * @param size New size of the box collider
     */
    void setSize(const glm::vec3 size);

    // Inherited via Collider
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

protected:
    // Inherited via Collider
    virtual physx::PxGeometry* genGeometry() const override;

private:
    glm::vec3 m_size = glm::vec3(1.0f);
};

} // namespace physics
} // namespace aderite
