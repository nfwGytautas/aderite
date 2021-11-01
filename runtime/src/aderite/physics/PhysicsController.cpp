#include "PhysicsController.hpp"

#include <PxActor.h>
#include <PxFoundation.h>
#include <PxMaterial.h>
#include <PxPhysics.h>
#include <PxPhysicsVersion.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <PxScene.h>
#include <PxSceneDesc.h>
#include <common/PxTolerancesScale.h>
#include <cooking/PxCooking.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxExtensionsAPI.h>
#include <foundation/PxAllocatorCallback.h>
#include <foundation/PxErrorCallback.h>
#include <foundation/PxErrors.h>
#include <pvd/PxPvd.h>
#include <pvd/PxPvdTransport.h>

#include "aderite/Aderite.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/ColliderList.hpp"
#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/components/Actors.hpp"
#include "aderite/scene/components/Meta.hpp"
#include "aderite/scene/components/Transform.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Class used to interface PhysX logging to SPDLOG aderite interface
 */
class PhysXErrorCallback : public physx::PxErrorCallback {
public:
    virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) {
        // Error processing implementation
        switch (code) {
        case physx::PxErrorCode::Enum::eDEBUG_INFO: {
            LOG_INFO("PhysX: {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::ePERF_WARNING: {
            LOG_WARN("PhysX: performance warning {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::eDEBUG_WARNING: {
            LOG_WARN("PhysX: {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::eINVALID_OPERATION:
        case physx::PxErrorCode::Enum::eOUT_OF_MEMORY:
        case physx::PxErrorCode::Enum::eINVALID_PARAMETER: {
            LOG_ERROR("PhysX: {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::eABORT:
        case physx::PxErrorCode::Enum::eINTERNAL_ERROR: {
            LOG_FATAL("PhysX: Uncoverable error {0} in {1}", message, file);
            break;
        }
        }
    }
};

static PhysXErrorCallback gErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;

bool PhysicsController::init() {
    // Foundation
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gErrorCallback);
    if (m_foundation == nullptr) {
        LOG_ERROR("Failed to create PhysX foundation");
        return false;
    }

    physx::PxTolerancesScale scale = physx::PxTolerancesScale();

    // PVD
    m_pvd = physx::PxCreatePvd(*m_foundation);
    if (m_pvd == nullptr) {
        LOG_ERROR("Failed to create PhysX visual debugger");
        return false;
    }

    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("", 5425, 10);
    if (transport == nullptr) {
        LOG_ERROR("Failed to create PhysX visual debugger transport");
        return false;
    }

    m_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    // Physics object
    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, scale, m_recordMemoryAllocations, m_pvd);
    if (m_physics == nullptr) {
        LOG_ERROR("Failed to create PhysX physics");
        return false;
    }

    // Cooking
    m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, physx::PxCookingParams(scale));
    if (m_cooking == nullptr) {
        LOG_ERROR("Failed to create PhysX cooking");
        return false;
    }

    // Extensions and dispatcher
    if (!PxInitExtensions(*m_physics, m_pvd)) {
        LOG_ERROR("Failed to init PhysX extensions");
        return false;
    }

    m_dispatcher = physx::PxDefaultCpuDispatcherCreate(m_numThreads);

    // Create default material
    m_defaultMaterial = m_physics->createMaterial(m_defaultStaticFriction, m_defaultDynamicFriction, m_defaultRestitution);

    // Configure collision filter
    physx::PxSetGroupCollisionFlag(0, 0, true);

    return true;
}

void PhysicsController::shutdown() {
    m_cooking->release();
    m_dispatcher->release();
    m_physics->release();
    m_pvd->release();
    PxCloseExtensions();
    m_foundation->release();
}

void PhysicsController::update(float delta) {
    auto currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    // Simulate a step
    if (currentScene == nullptr) {
        return;
    }

    physics::PhysicsScene* physicsScene = currentScene->getPhysicsScene();
    if (physicsScene == nullptr) {
        return;
    }

    // Initialize if not already initialized
    if (!physicsScene->initialized()) {
        physicsScene->initialize(this);
    }

    // Properties
    this->syncProperties();

    // Remove detached
    this->removeDetached();

    // Simulate
    uint8_t substeps = delta / c_SubStepLength;
    for (uint8_t i = 0; i < substeps; i++) {
        physicsScene->simulate(c_SubStepLength);
        this->syncChanges();
    }
    float remainder = delta - (substeps * c_SubStepLength);
    physicsScene->simulate(remainder);
    this->syncChanges();
}

physx::PxRigidStatic* PhysicsController::createStaticBody() {
    return m_physics->createRigidStatic(physx::PxTransform(physx::PxVec3(0)));
}

physx::PxRigidDynamic* PhysicsController::createDynamicBody() {
    return m_physics->createRigidDynamic(physx::PxTransform(physx::PxVec3(0)));
}

physx::PxPhysics* PhysicsController::getPhysics() {
    return m_physics;
}

physx::PxCpuDispatcher* PhysicsController::getDispatcher() {
    return m_dispatcher;
}

physx::PxMaterial* PhysicsController::getDefaultMaterial() {
    return m_defaultMaterial;
}

physx::PxFilterFlags PhysicsController::filterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
                                                     physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
                                                     physx::PxPairFlags& pairFlags, const void* constantBlock,
                                                     physx::PxU32 constantBlockSize) {
    // Let triggers through
    if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1)) {
        pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
        return physx::PxFilterFlag::eDEFAULT;
    }

    // Generate contacts for all that were not filtered above
    pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
    pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_LOST;

    return physx::PxFilterFlag::eDEFAULT;
}

void PhysicsController::syncChanges() {
    auto currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    // Sync changes with ECS
    auto dynamicGroup = currentScene->getEntityRegistry().group<scene::DynamicActor>(entt::get<scene::TransformComponent>);
    for (auto entity : dynamicGroup) {
        auto [actor, transform] = dynamicGroup.get(entity);

        // Sync transform
        actor.Actor->sync(transform);
    }
}

void PhysicsController::syncProperties() {
    auto currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
    auto physicsScene = currentScene->getPhysicsScene();

    // Sync changes with ECS
    auto dynamicView = currentScene->getEntityRegistry().group<scene::DynamicActor>(entt::get<scene::CollidersComponent>);
    for (auto entity : dynamicView) {
        auto [actor, colliders] = dynamicView.get(entity);

        if (actor.Actor == nullptr) {
            // Create actor
            actor.Actor = new physics::DynamicActor();
            actor.Actor->init(this);

            // Set actor for colliders
            actor.Actor->setColliders(colliders.Colliders);

            // Add to the physics scene
            physicsScene->addActor(actor.Actor);

            // Track
            m_actors.add(actor.Actor);
        }
    }

    auto staticView = currentScene->getEntityRegistry().group<scene::StaticActor>(entt::get<scene::CollidersComponent>);
    for (auto entity : staticView) {
        auto [actor, colliders] = staticView.get(entity);

        if (actor.Actor == nullptr) {
            // Create actor
            actor.Actor = new physics::StaticActor();
            actor.Actor->init(this);

            // Set actor for colliders
            actor.Actor->setColliders(colliders.Colliders);

            // Add to the physics scene
            physicsScene->addActor(actor.Actor);

            // Track
            m_actors.add(actor.Actor);
        }
    }
}

void PhysicsController::removeDetached() {
    // TODO: Iterate and remove actors
}

} // namespace physics
} // namespace aderite
