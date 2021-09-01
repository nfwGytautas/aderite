#include "PhysicsController.hpp"

#include <PxPhysics.h>
#include <PxFoundation.h>
#include <PxPhysicsVersion.h>
#include <PxScene.h>
#include <PxSceneDesc.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <PxMaterial.h>
#include <PxActor.h>
#include <pvd/PxPvd.h>
#include <pvd/PxPvdTransport.h>
#include <cooking/PxCooking.h>
#include <common/PxTolerancesScale.h>
#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <foundation/PxErrorCallback.h>
#include <foundation/PxErrors.h>
#include <foundation/PxAllocatorCallback.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/ColliderList.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"

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
    if (currentScene != nullptr) {
        // TODO: Substep
        currentScene->fixedUpdate(delta);
    }
}

physx::PxRigidStatic* PhysicsController::createStaticBody() {
    return m_physics->createRigidStatic(physx::PxTransform(physx::PxVec3(0)));
}

physx::PxRigidDynamic* PhysicsController::createDynamicBody() {
    return m_physics->createRigidDynamic(physx::PxTransform(physx::PxVec3(0)));;
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

physx::PxFilterFlags PhysicsController::filterShader(
    physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
    physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
    physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
    // Let triggers through
    if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
        return physx::PxFilterFlag::eDEFAULT;
    }

    // Generate contacts for all that were not filtered above
    pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
    pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_LOST;

    return physx::PxFilterFlag::eDEFAULT;
}

ADERITE_PHYSICS_NAMESPACE_END
