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
#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsEventList.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/EntitySelector.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scripting/LibClassLocator.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"

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
            LOG_INFO("[Physics] PhysX: {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::ePERF_WARNING: {
            LOG_WARN("[Physics] PhysX: performance warning {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::eDEBUG_WARNING: {
            LOG_WARN("[Physics] PhysX: {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::eINVALID_OPERATION:
        case physx::PxErrorCode::Enum::eOUT_OF_MEMORY:
        case physx::PxErrorCode::Enum::eINVALID_PARAMETER: {
            LOG_ERROR("[Physics] PhysX: {0} in {1}", message, file);
            break;
        }
        case physx::PxErrorCode::Enum::eABORT:
        case physx::PxErrorCode::Enum::eINTERNAL_ERROR: {
            LOG_FATAL("[Physics] PhysX: Uncoverable error {0} in {1}", message, file);
            break;
        }
        }
    }
};

static PhysXErrorCallback gErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;

bool PhysicsController::init() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Physics] Initializing physics controller");

    // Foundation
    LOG_TRACE("[Physics] Creating PhysX foundation");
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gErrorCallback);
    if (m_foundation == nullptr) {
        LOG_ERROR("[Physics] Failed to create PhysX foundation");
        return false;
    }
    LOG_INFO("[Physics] PhysX foundation created");

    physx::PxTolerancesScale scale = physx::PxTolerancesScale();

#ifdef _DEBUG
    // PVD
    LOG_TRACE("[Physics] Creating PhysX PVD");
    m_pvd = physx::PxCreatePvd(*m_foundation);
    if (m_pvd == nullptr) {
        LOG_ERROR("[Physics] Failed to create PhysX visual debugger");
        return false;
    }
    LOG_INFO("[Physics] PhysX PVD created");

    LOG_TRACE("[Physics] Creating PhysX PVD transport");
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("", 5425, 10);
    if (transport == nullptr) {
        LOG_ERROR("[Physics] Failed to create PhysX visual debugger transport");
        return false;
    }
    LOG_INFO("[Physics] PhysX PVD transport created");

    m_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif

    // Physics object
    LOG_TRACE("[Physics] Creating PhysX physics object");
    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, scale, m_recordMemoryAllocations, m_pvd);
    if (m_physics == nullptr) {
        LOG_ERROR("[Physics] Failed to create PhysX physics");
        return false;
    }
    LOG_INFO("[Physics] PhysX physics object created");

    // Cooking
    LOG_TRACE("[Physics] Creating PhysX cooking library");
    m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, physx::PxCookingParams(scale));
    if (m_cooking == nullptr) {
        LOG_ERROR("[Physics] Failed to create PhysX cooking");
        return false;
    }
    LOG_INFO("[Physics] PhysX cooking library created");

    // Extensions and dispatcher
    LOG_TRACE("[Physics] Initializing PhysX extensions");
    if (!PxInitExtensions(*m_physics, m_pvd)) {
        LOG_ERROR("[Physics] Failed to init PhysX extensions");
        return false;
    }
    LOG_INFO("[Physics] PhysX extensions initialized");

    LOG_TRACE("[Physics] Setting up physics defaults and properties");
    m_dispatcher = physx::PxDefaultCpuDispatcherCreate(m_numThreads);

    // Create default material
    m_defaultMaterial = m_physics->createMaterial(m_defaultStaticFriction, m_defaultDynamicFriction, m_defaultRestitution);

    // Configure collision filter
    physx::PxSetGroupCollisionFlag(0, 0, true);

    // Create event list
    m_events = new PhysicsEventList();

    LOG_INFO("[Physics] Physics controller initialized");

    return true;
}

void PhysicsController::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Physics] Shutting down physics controller");

    m_cooking->release();
    m_dispatcher->release();
    m_physics->release();
    m_pvd->release();
    PxCloseExtensions();
    m_foundation->release();

    delete m_events;

    LOG_INFO("[Physics] Physics controller shutdown");
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

    // Remove detached
    this->removeDetached();

    m_accumulator += delta;

    if (m_accumulator < c_FixedUpdateWindow) {
        return;
    }

    m_accumulator -= c_FixedUpdateWindow;
    physicsScene->simulate(c_FixedUpdateWindow);
    this->syncChanges();
}

PhysicsEventList* PhysicsController::getEventList() const {
    return m_events;
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

    PhysicsScene* scene = currentScene->getPhysicsScene();

    for (physics::PhysicsActor* actor : scene->getActors()) {
        actor->sync();
    }

    // Send physics events to scripts
    auto& locator = ::aderite::Engine::getScriptManager()->getLocator();

    for (const TriggerEvent& te : m_events->getTriggerEvents()) {
        MonoObject* triggerObject = locator.create(te);
        for (scripting::ScriptSystem* system : currentScene->getScriptSystems()) {
            scene::EntitySelector* selector = system->getSelector();

            if (selector == nullptr) {
                // No selector so system doesn't receive any entity events
                continue;
            }

            if (!selector->isSelected(te.Actor->getEntity()) && !selector->isSelected(te.Trigger->getEntity())) {
                // System is not interested in either of the entities
                continue;
            }

            if (te.Enter) {
                system->triggerEnter(triggerObject);
            } else {
                system->triggerLeave(triggerObject);
            }
        }
    }

    for (const CollisionEvent& ce : m_events->getCollisionEvents()) {
        MonoObject* collisionObject = locator.create(ce);
        for (scripting::ScriptSystem* system : currentScene->getScriptSystems()) {
            scene::EntitySelector* selector = system->getSelector();

            if (selector == nullptr) {
                // No selector so system doesn't receive any entity events
                continue;
            }

            if (!selector->isSelected(ce.Actor1->getEntity()) && !selector->isSelected(ce.Actor2->getEntity())) {
                // System is not interested in either of the entities
                continue;
            }

            if (ce.Start) {
                system->collisionStart(collisionObject);
            } else {
                system->collisionEnd(collisionObject);
            }
        }
    }

    // Clear events
    m_events->clear();
}

void PhysicsController::removeDetached() {
    // TODO: Iterate and remove actors
    // TODO: Also detach shapes from the actor
}

} // namespace physics
} // namespace aderite
