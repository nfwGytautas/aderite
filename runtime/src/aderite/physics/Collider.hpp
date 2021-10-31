#pragma once

#include <vector>

#include <PxShape.h>
#include <glm/glm.hpp>

#include "aderite/asset/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Collider base class
 */
class Collider : public io::ISerializable {
public:
    /**
     * @brief Creates a collider from geometry
     * @param geometry Geometry of the collider (will be deleted)
     */
    Collider(physx::PxGeometry* geometry);

    virtual ~Collider();

    /**
     * @brief Returns true if the collider acts as a trigger or not
     */
    bool isTrigger() const;

    /**
     * @brief Converts the collider to a trigger if true, if false converts to collider
     */
    void setTrigger(bool value);

    /**
     * @brief Sets the actor of the collider, this will remove the collider from previous actor
     * @param actor New actor of the collider
     */
    void setActor(physx::PxRigidActor* actor);

    /**
     * @brief Sets the scale of the collider
     * @param scale New scale of the collider
     */
    virtual void setScale(const glm::vec3& scale) = 0;

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

protected:
    // Not serialized
    physx::PxShape* p_shape = nullptr;
};

} // namespace physics
} // namespace aderite
