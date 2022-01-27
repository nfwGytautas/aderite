#pragma once

#include <vector>

#include <physx/PxRigidActor.h>

#include "aderite/io/ISerializable.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/physics/PhysicsProperties.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scene/TransformProvider.hpp"

namespace aderite {
namespace physics {

/**
 * @brief A wrapper for PhysX physics engine
 */
class PhysXActor final : public scene::TransformProvider, public io::ISerializable {
public:
    PhysXActor(scene::GameObject* gObject);
    ~PhysXActor();

    /**
     * @brief Make the actor static
     */
    void makeStatic();

    /**
     * @brief Make the actor dynamic
     */
    void makeDynamic();

    /**
     * @brief Returns true if the actor is dynamic, false otherwise
     */
    bool isDynamic() const;

    /**
     * @brief Applies physics properties to the actor
     * @param properties Properties to apply
     */
    void applyProperties(const PhysicsProperties& properties);

    /**
     * @brief Gets the physics properties of the actor
     * @return PhysicsProperties struct
     */
    PhysicsProperties getProperties() const;

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

    /**
     * @brief Returns the PhysX actor instance
     */
    physx::PxRigidActor* getActor() const;

    // Inherited via TransformProvider
    virtual const glm::vec3& getPosition() const override;
    virtual const glm::quat& getRotation() const override;
    virtual const glm::vec3& getScale() const override;
    virtual void setPosition(const glm::vec3& position) override;
    virtual void setRotation(const glm::quat& rotation) override;
    virtual void setScale(const glm::vec3& scale) override;

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    /**
     * @brief Frees all attached geometry of the actor
     */
    void freeGeometry();

    /**
     * @brief Transfers all geometry to another actor
     * @param actor Actor to transfer to
     */
    void transferGeometry(physx::PxRigidActor* actor);

    /**
     * @brief Frees the PhysX actor
     */
    void freeActor();

private:
    scene::GameObject* m_gObject = nullptr;
    bool m_isDynamic = false;
    physx::PxRigidActor* m_actor = nullptr;
    glm::vec3 m_scale = {1.0f, 1.0f, 1.0f};
};

} // namespace physics
} // namespace aderite
