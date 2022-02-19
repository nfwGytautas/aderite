#pragma once

#include <string>

#include <glm/glm.hpp>
#include <physx/PxShape.h>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Geometry is the interface class for geometry types for collider shapes
 */
class Geometry : public io::SerializableObject {
public:
    Geometry();
    virtual ~Geometry();

    /**
     * @brief Returns the actor of the geometry
     */
    PhysXActor* getActor() const;

    /**
     * @brief Returns the PhysX shape of the geometry
     */
    physx::PxShape* getShape() const;

    /**
     * @brief Returns true if this geometry is used as a trigger, false otherwise
     */
    bool isTrigger() const;

    /**
     * @brief Sets how the geometry should be used, if true then this geometry will become a trigger if false then a collider
     * @param value New value of the geometry
     */
    void setTrigger(bool value);

    /**
     * @brief Apply scale to geometry
     * @param scale Scale to apply
     */
    virtual void applyScale(const glm::vec3& scale) = 0;

    /**
     * @brief Clone the geometry and return the new instance
     */
    virtual Geometry* clone() = 0;

protected:
    physx::PxShape* p_shape = nullptr;
};

} // namespace physics
} // namespace aderite
