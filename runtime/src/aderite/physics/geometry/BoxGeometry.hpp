#pragma once

#include "aderite/physics/geometry/Geometry.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Simple 3D box geometry
 */
class BoxGeometry : public Geometry {
public:
    BoxGeometry();

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

    // Inherited via Geometry
    void applyScale(const glm::vec3& scale) override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
    reflection::Type getType() const override;

private:
    glm::vec3 m_size = glm::vec3(1.0f);

};

} // namespace physics
} // namespace aderite
