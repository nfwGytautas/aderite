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
     * @brief Returns true if the collider acts as a trigger or not
     */
    bool isTrigger() const;

    /**
     * @brief Converts the collider to a trigger if true, if false converts to collider
     */
    void setTrigger(bool value);

    /**
     * @brief Attach collider to actor
     * @param actor Actor to attach to
     * @param globalScale Scale to be applied
     */
    void attach(PhysicsActor* actor, const glm::vec3& globalScale);

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

protected:
    /**
     * @brief Generate a geometry with specified globalScale
     * @param globalScale Global scale of the geometry
     * @return physx::PxGeometry instance
     */
    virtual physx::PxGeometry* genGeometry(const glm::vec3& globalScale) = 0;

private:
    /**
     * @brief Creates physx shape from collider geometry
     * @param globalScale Scale to be applied
     * @return PxShape instance
     */
    physx::PxShape* createShape(const glm::vec3& globalScale);

private:
    bool m_isTrigger = false;
};

} // namespace physics
} // namespace aderite
