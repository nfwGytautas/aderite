#pragma once

#include <PxShape.h>
#include <glm/glm.hpp>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Collider base class
 */
class Collider : public io::ISerializable {
public:
    Collider();
    virtual ~Collider();

    /**
     * @brief Set the scale of the collider
     * @param scale New scale
     */
    void setScale(const glm::vec3& scale);

    /**
     * @brief Returns the scale of the collider
     */
    const glm::vec3& getScale() const {
        return p_scale;
    }

    /**
     * @brief Returns true if the collider acts as a trigger or not
     */
    bool isTrigger() const;

    /**
     * @brief Converts the collider to a trigger if true, if false converts to collider
     */
    void setTrigger(bool value);

    /**
     * @brief Sets the actor of the shape
     */
    void setActor(physx::PxRigidActor* actor);

    /**
     * @brief Returns the actor of the collider
     */
    physics::PhysicsActor* getActor() const;

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

protected:
    /**
     * @brief Generate a geometry with specified globalScale
     * @return physx::PxGeometry instance
     */
    virtual physx::PxGeometry* genGeometry() = 0;

    /**
     * @brief Updates the geometry of the shape
     */
    void updateGeometry();

    /**
     * @brief Creates the shape of the collider
     */
    void createShape();

protected:
    physx::PxShape* p_shape = nullptr;
    glm::vec3 p_scale = {1.0f, 1.0f, 1.0f};
};

} // namespace physics
} // namespace aderite
