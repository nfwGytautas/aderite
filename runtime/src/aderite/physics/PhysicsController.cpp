#include "PhysicsController.hpp"

#include <PxPhysics.h>
#include <PxFoundation.h>
#include <PxPhysicsVersion.h>
#include <PxScene.h>
#include <PxSceneDesc.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
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
#include "aderite/physics/ColliderList.hpp"
#include "aderite/physics/Collider.hpp"
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

	return true;
}

void PhysicsController::shutdown() {
    for (auto trigger : m_triggers) {
        trigger->release();
    }

    for (auto cl : m_colliderLists) {
        delete cl;
    }

    if (m_scene) {
        m_scene->release();
    }

    m_cooking->release();
    m_dispatcher->release();
    m_physics->release();
    m_pvd->release();
    PxCloseExtensions();
    m_foundation->release();
}

void PhysicsController::update(float delta) {
    // Sync changes to transforms
    auto group = ::aderite::Engine::getSceneManager()->getCurrentScene()->getEntityRegistry()
        .group<scene::components::RigidbodyComponent>(entt::get<scene::components::TransformComponent, scene::components::CollidersComponent>);
    for (auto entity : group) {
        auto [rigidbody, transform] = group.get<scene::components::RigidbodyComponent, scene::components::TransformComponent>(entity);

        if (transform.WasAltered) {
            // Sync ECS -> PhysX
            syncActor(rigidbody, transform);
            transform.WasAltered = false;
        }
        else if (!rigidbody.IsStatic) {
            // Sync ECS <- PhysX
            syncTransform(rigidbody, transform);
        }
    }

    // Simulate a step
    m_scene->simulate(delta);
    m_scene->fetchResults(true);

    //// Retrieve array of actors that moved
    //physx::PxU32 nbActiveActors;
    //physx::PxActor** activeActors = m_scene->getActiveActors(nbActiveActors);
    //
    //// Update each render object with the new transform
    //for (physx::PxU32 i = 0; i < nbActiveActors; ++i)
    //{
    //    // Sync PhysX -> ECS
    //    Rigidbody* rbody = static_cast<Rigidbody*>(activeActors[i]->userData);
    //    rbody->syncTransform();
    //}
}

void PhysicsController::reset() {
    m_isCreating = true;

    // Free last scene
    if (m_scene != nullptr) {
        m_scene->release();
        m_scene = nullptr;
    }

    for (auto trigger : m_triggers) {
        trigger->release();
    }
    m_triggers.clear();

    // Create scene now
    physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = m_dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    sceneDesc.flags = physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    m_scene = m_physics->createScene(sceneDesc);

    if (m_scene == nullptr) {
        LOG_ERROR("Failed to create a PhysX scene");
        return;
    }

    // Add all objects (Rigidbody, transform and colliders)
    auto objectGroup = ::aderite::Engine::getSceneManager()->getCurrentScene()->getEntityRegistry()
        .group<scene::components::RigidbodyComponent>(entt::get<scene::components::TransformComponent, scene::components::CollidersComponent>);
    for (auto entity : objectGroup) {
        auto [rigidbody, colliders, transform] = objectGroup.get<
            scene::components::RigidbodyComponent, 
            scene::components::CollidersComponent, 
            scene::components::TransformComponent>(entity);

        if (rigidbody.IsStatic) {
            createStaticbody(rigidbody, colliders, transform);
        }
        else {
            createRigidbody(rigidbody, colliders, transform);
        }
    }

    // Add all triggers (transform and colliders)
    auto triggerGroup = ::aderite::Engine::getSceneManager()->getCurrentScene()->getEntityRegistry()
        .group<scene::components::CollidersComponent>(
            entt::get<scene::components::TransformComponent>, 
            entt::exclude<scene::components::RigidbodyComponent>);
    for (auto entity : triggerGroup) {
        auto [colliders, transform] = triggerGroup.get<scene::components::CollidersComponent, scene::components::TransformComponent>(entity);
        createTrigger(colliders, transform);
    }

    m_isCreating = false;
}

ColliderList* PhysicsController::newColliderList() {
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

void PhysicsController::createRigidbody(
    scene::components::RigidbodyComponent& rbody, 
    const scene::components::CollidersComponent& colliders, 
    const scene::components::TransformComponent& transform) 
{
    physx::PxRigidDynamic* actor = m_physics->createRigidDynamic(physx::PxTransform(physx::PxVec3(0)));
    actor->setMass(rbody.Mass);
    rbody.Actor = actor;
    setupBody(actor, rbody, colliders, transform);
}

void PhysicsController::createStaticbody(
    scene::components::RigidbodyComponent& rbody,
    const scene::components::CollidersComponent& colliders,
    const scene::components::TransformComponent& transform)
{
    physx::PxRigidStatic* actor = m_physics->createRigidStatic(physx::PxTransform(physx::PxVec3(0)));
    rbody.Actor = actor;
    setupBody(actor, rbody, colliders, transform);
}

void PhysicsController::setupBody(
    physx::PxRigidActor* actor,
    const scene::components::RigidbodyComponent& rbody, 
    const scene::components::CollidersComponent& colliders, 
    const scene::components::TransformComponent& transform) 
{
    // TODO: Material
    actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !rbody.HasGravity);
    m_scene->addActor(*actor);

    // Set transform
    syncActor(rbody, transform);

    // Create colliders and attach them
    for (Collider* c : *colliders.Colliders) {
        physx::PxShape* shape = c->construct(m_physics, m_defaultMaterial);
        if (c->isTrigger()) {
            m_triggers.push_back(shape);
        }
        else {
            actor->attachShape(*shape);
        }
    }
}

void PhysicsController::createTrigger(
    const scene::components::CollidersComponent& colliders, 
    const scene::components::TransformComponent& transform) 
{
    ADERITE_UNIMPLEMENTED;
}

void PhysicsController::syncActor(
    const scene::components::RigidbodyComponent& rbody, 
    const scene::components::TransformComponent& transform) 
{
    physx::PxRigidBody* actor = static_cast<physx::PxRigidBody*>(rbody.Actor);

    // Suppress if currently creating the scene
    if (rbody.IsStatic && !m_isCreating) {
        LOG_WARN("syncActor called on a static body, consider removing IsStatic flag");
    }

    actor->setGlobalPose(
        physx::PxTransform(
            physx::PxVec3{ transform.Position.x, transform.Position.y, transform.Position.z },
            physx::PxQuat{ transform.Rotation.x, transform.Rotation.y, transform.Rotation.z, transform.Rotation.w }
    ));
}

void PhysicsController::syncTransform(const scene::components::RigidbodyComponent& rbody, scene::components::TransformComponent& transform) {
    physx::PxRigidBody* actor = static_cast<physx::PxRigidBody*>(rbody.Actor);
    physx::PxTransform pxt = actor->getGlobalPose();
    transform.Position = { pxt.p.x, pxt.p.y, pxt.p.z };
    transform.Rotation = { pxt.q.w,  pxt.q.x, pxt.q.y, pxt.q.z };
}

ADERITE_PHYSICS_NAMESPACE_END
