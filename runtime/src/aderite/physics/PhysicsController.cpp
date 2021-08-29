#include "PhysicsController.hpp"

#include <PxPhysics.h>
#include <PxFoundation.h>
#include <PxPhysicsVersion.h>
#include <PxScene.h>
#include <PxSceneDesc.h>
#include <PxRigidDynamic.h>
#include <cooking/PxCooking.h>
#include <common/PxTolerancesScale.h>
#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <foundation/PxErrorCallback.h>
#include <foundation/PxErrors.h>
#include <foundation/PxAllocatorCallback.h>

#include "aderite/utility/Log.hpp"
#include "aderite/physics/Rigidbody.hpp"
#include "aderite/physics/ColliderList.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

/**
 * @brief Class used to interface PhysX logging to SPDLOG aderite interface
*/
class PhysXErrorCallback : public physx::PxErrorCallback {
public:
    virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) {
        // Error processing implementation
        switch (code) {
        case physx::PxErrorCode::Enum::eDEBUG_INFO: {
            LOG_INFO("Physics: {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::ePERF_WARNING: {
            LOG_WARN("Physics: performance warning {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::eDEBUG_WARNING: {
            LOG_WARN("Physics: {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::eINVALID_OPERATION:
        case physx::PxErrorCode::Enum::eOUT_OF_MEMORY:
        case physx::PxErrorCode::Enum::eINVALID_PARAMETER: {
            LOG_ERROR("Physics: {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::eABORT:
        case physx::PxErrorCode::Enum::eINTERNAL_ERROR: {
            LOG_FATAL("Physics: Uncoverable error {0} in {1}", message, file);
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
    //mPvd = PxCreatePvd(*gFoundation);
    //PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    //mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    // Physics object
    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, scale, m_recordMemoryAllocations, nullptr);
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
    if (!PxInitExtensions(*m_physics, nullptr)) {
        LOG_ERROR("Failed to init PhysX extensions");
        return false;
    }

    m_dispatcher = physx::PxDefaultCpuDispatcherCreate(m_numThreads);

    // Create scene now
    physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = m_dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    sceneDesc.flags = physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    m_scene = m_physics->createScene(sceneDesc);

    if (m_scene == nullptr) {
        LOG_ERROR("Failed to create a PhysX scene");
        return false;
    }

    // Create default material
    m_defaultMaterial = m_physics->createMaterial(m_defaultStaticFriction, m_defaultDynamicFriction, m_defaultRestitution);

	return true;
}

void PhysicsController::shutdown() {
    for (auto rbody : m_bodies) {
        delete rbody;
    }

    for (auto cl : m_colliderLists) {
        delete cl;
    }

    m_scene->release();
    m_cooking->release();
    m_dispatcher->release();
    m_physics->release();

    PxCloseExtensions();

    m_foundation->release();
}

void PhysicsController::update(float delta) {
    m_scene->simulate(delta);
    m_scene->fetchResults(true);

    // Retrieve array of actors that moved
    physx::PxU32 nbActiveActors;
    physx::PxActor** activeActors = m_scene->getActiveActors(nbActiveActors);

    // Update each render object with the new transform
    for (physx::PxU32 i = 0; i < nbActiveActors; ++i)
    {
        Rigidbody* rbody = static_cast<Rigidbody*>(activeActors[i]->userData);
        rbody->update(delta);
    }
}

void PhysicsController::attachRigidBody(scene::Entity on) {
    Rigidbody* rbody = new Rigidbody(this, on);
    m_scene->addActor(*rbody->m_rbody);
    m_bodies.push_back(rbody);
}

ColliderList* PhysicsController::beginNewColliderList() {
    ColliderList* cl = new ColliderList();
    m_colliderLists.push_back(cl);
    return cl;
}

physx::PxPhysics* PhysicsController::getPhysics() {
    return m_physics;
}

physx::PxMaterial* PhysicsController::getDefaultMaterial() {
    return m_defaultMaterial;
}

ADERITE_PHYSICS_NAMESPACE_END
