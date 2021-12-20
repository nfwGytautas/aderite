#pragma once

#include <string>

#include <glm/glm.hpp>
#include <physx/PxShape.h>

#include "aderite/scripting/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"

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
     * @brief Function invoked by the physics scene when this geometry had a physics event happen to it, specifically a enter
     * @param other Geometry instance that was collided/entered
     */
    void onPhysicsEventEnter(Geometry* other) const;

    /**
     * @brief Function invoked by the physics scene when this geometry had a physics event happen to it, specifically a leave
     * @param other Geometry instance that was collided/entered
     */
    void onPhysicsEventLeave(Geometry* other) const;

    /**
     * @brief Set the C# event to be called when another geometry entered it
     * @param e Event instance
    */
    void setEnterEvent(scripting::ScriptGeometryEvent* e);

    /**
     * @brief Get the C# event instance, nullptr if not set
    */
    scripting::ScriptGeometryEvent* getEnterEvent();

    /**
     * @brief Set the C# event to be called when another geometry left it
     * @param e Event instance
     */
    void setLeaveEvent(scripting::ScriptGeometryEvent* e);

    /**
     * @brief Get the C# event instance, nullptr if not set
     */
    scripting::ScriptGeometryEvent* getLeaveEvent();

    /**
     * @brief Apply scale to geometry
     * @param scale Scale to apply
     */
    virtual void applyScale(const glm::vec3& scale) = 0;

    // Inherited from SerializableObject
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

protected:
    // Needed to access p_shape
    friend class BasePhysicsActor;

    physx::PxShape* p_shape = nullptr;

    // Events
    scripting::ScriptGeometryEvent* m_enter = nullptr;
    scripting::ScriptGeometryEvent* m_leave = nullptr;
};

} // namespace physics
} // namespace aderite
