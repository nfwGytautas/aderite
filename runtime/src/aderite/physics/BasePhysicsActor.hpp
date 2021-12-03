#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <physx/PxRigidActor.h>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/ITransformProvider.h"

namespace aderite {
namespace physics {

/**
 * @brief BasePhysicsActor class is a base class for dynamic and static physics actors
 */
class BasePhysicsActor : public scene::ITransformProvider, public io::ISerializable {
public:
    virtual ~BasePhysicsActor();

    /**
     * @brief Add geometry to the actor
     * @param geometry Geometry instance
     */
    void addGeometry(Geometry* geometry);

    /**
     * @brief Remove geometry from the actor
     * @param geometry Geometry to remove
     */
    void removeGeometry(Geometry* geometry);

    /**
     * @brief Returns a list of attached geometries on this actor
     */
    std::vector<Geometry*> getAttachedGeometries() const;

    // Inherited via ITransformProvider
    const glm::vec3& getPosition() const override;
    const glm::quat& getRotation() const override;
    const glm::vec3& getScale() const override;
    void setPosition(const glm::vec3& position) override;
    void setRotation(const glm::quat& rotation) override;
    void setScale(const glm::vec3& scale) override;

    // Inherited via ISerializable
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

public:
    // Needed to access p_actor
    friend class PhysicsScene;

private:
    /**
     * @brief Frees all attached geometry of the actor
     */
    void freeGeometry();

protected:
    // Force to be "abstract"
    BasePhysicsActor();

    physx::PxRigidActor* p_actor = nullptr;

private:
    glm::vec3 m_scale = {1.0f, 1.0f, 1.0f};
};

} // namespace physics
} // namespace aderite
