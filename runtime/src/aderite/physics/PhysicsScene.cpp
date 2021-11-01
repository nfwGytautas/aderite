#include "PhysicsScene.hpp"

#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/EventNotifier.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace physics {

PhysicsScene::PhysicsScene() {}

PhysicsScene::~PhysicsScene() {
    if (m_scene != nullptr) {
        m_scene->release();
    }
}

void PhysicsScene::simulate(float step) {
    m_scene->simulate(step);
    m_scene->fetchResults(true);
}

void PhysicsScene::initialize(physics::PhysicsController* controller) {
    auto physics = controller->getPhysics();

    // TODO: Configure physics properties
    physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = controller->getDispatcher();
    sceneDesc.filterShader = physics::PhysicsController::filterShader;
    sceneDesc.simulationEventCallback = this;
    // sceneDesc.flags = physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    m_scene = physics->createScene(sceneDesc);
    ADERITE_DYNAMIC_ASSERT(m_scene != nullptr, "Failed to create a PhysX scene");
}

void PhysicsScene::addActor(PhysicsActor* actor) {
    m_scene->addActor(*actor->p_actor);
}

bool PhysicsScene::initialized() const {
    return m_scene != nullptr;
}

void PhysicsScene::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {
    for (physx::PxU32 i = 0; i < nbPairs; i++) {
        const physx::PxContactPair& cp = pairs[i];

        physx::PxRigidActor* actor1 = pairHeader.actors[0];
        physx::PxRigidActor* actor2 = pairHeader.actors[1];

        PhysicsActor* e1 = static_cast<PhysicsActor*>(actor1->userData);
        PhysicsActor* e2 = static_cast<PhysicsActor*>(actor2->userData);

        // Get notifiers
        EventNotifier* e1Notifier = e1->getNotifier();
        EventNotifier* e2Notifier = e2->getNotifier();

        // Send notifications
        if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            if (e1Notifier != nullptr) {
                e1Notifier->onCollisionEnter(e2);
            }

            if (e2Notifier != nullptr) {
                e2Notifier->onCollisionEnter(e1);
            }
        } else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            if (e1Notifier != nullptr) {
                e1Notifier->onCollisionLeave(e2);
            }

            if (e2Notifier != nullptr) {
                e2Notifier->onCollisionLeave(e1);
            }
        }
    }
}

void PhysicsScene::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs) {
    for (physx::PxU32 i = 0; i < nbPairs; i++) {
        const physx::PxTriggerPair& cp = pairs[i];

        physx::PxRigidActor* actor = cp.otherActor;
        physx::PxRigidActor* trigger = cp.triggerActor;

        PhysicsActor* paA = static_cast<PhysicsActor*>(actor->userData);
        PhysicsActor* paT = static_cast<PhysicsActor*>(trigger->userData);

        // Get notifiers
        EventNotifier* aNotifier = paA->getNotifier();
        EventNotifier* tNotifier = paT->getNotifier();

        // Send notifications
        if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            if (aNotifier != nullptr) {
                aNotifier->onTriggerEnter(paT);
            }

            if (tNotifier != nullptr) {
                tNotifier->onTriggerEnter(paA);
            }
        } else if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            if (aNotifier != nullptr) {
                aNotifier->onTriggerLeave(paT);
            }

            if (tNotifier != nullptr) {
                tNotifier->onTriggerLeave(paA);
            }
        }
    }
}

} // namespace physics
} // namespace aderite