#pragma once

#include "aderite/physics/Collider.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Simple box collider
 */
class BoxCollider : public Collider {
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
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

protected:
    // Inherited via Collider
    virtual physx::PxGeometry* genGeometry() override;

private:
    glm::vec3 m_size = glm::vec3(1.0f);
};

} // namespace physics
} // namespace aderite
