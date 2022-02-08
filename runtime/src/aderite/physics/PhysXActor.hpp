#pragma once

#include <vector>

#include <physx/PxRigidActor.h>

#include "aderite/physics/Forward.hpp"
#include "aderite/physics/PhysicsProperties.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scene/TransformProvider.hpp"

namespace aderite {
namespace physics {

/**
 * @brief A wrapper for PhysX physics engine
 */
class PhysXActor final {
public:
    PhysXActor(scene::GameObject* gObject);
    ~PhysXActor();

    /**
     * @brief Update the renderable properties
     * @param delta Delta time of last frame
     */
    void update(float delta);

    /**
     * @brief Returns the PhysX actor instance
     */
    physx::PxRigidActor* getActor() const;

    /**
     * @brief Returns the properties of this actor
     */
    PhysicsProperties& getData();

private:
    /**
     * @brief Transfers all geometry to another actor
     * @param actor Actor to transfer to
     */
    void transferGeometry(physx::PxRigidActor* actor);

    /**
     * @brief Frees the PhysX actor
     */
    void freeActor();

    /**
     * @brief Create PhysX actor
     */
    void createActor();

private:
    scene::GameObject* m_gObject = nullptr;
    physx::PxRigidActor* m_actor = nullptr;
    PhysicsProperties m_properties;
    bool m_isDynamic = false; // Used to track state change
};

} // namespace physics
} // namespace aderite
