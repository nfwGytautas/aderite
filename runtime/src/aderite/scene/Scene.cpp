#include "Scene.hpp"
#include <algorithm>
#include <fstream>

#include <PxRigidActor.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/physics/ColliderList.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/scripting/ScriptList.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace scene {

void serialize_entity(YAML::Emitter& out, Entity e) {
    out << YAML::BeginMap; // Entity

    out << YAML::Key << "Entity" << YAML::Value << (entt::id_type)e;

    // Meta component
    if (e.hasComponent<MetaComponent>()) {
        out << YAML::Key << "Meta";
        out << YAML::BeginMap; // Meta

        MetaComponent& mc = e.getComponent<MetaComponent>();
        out << YAML::Key << "Name" << YAML::Value << mc.Name;

        out << YAML::EndMap; // Meta
    } else {
        LOG_ERROR("Tried to serialize Entity without MetaComponent component, this should never happen");
        return;
    }

    // Serialize rest of components

    // Transform
    if (e.hasComponent<TransformComponent>()) {
        out << YAML::Key << "Transform";
        out << YAML::BeginMap; // Transform

        TransformComponent& tc = e.getComponent<TransformComponent>();
        out << YAML::Key << "Position" << YAML::Value << tc.Position;
        out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
        out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

        out << YAML::EndMap; // Transform
    }

    // Mesh Renderer
    if (e.hasComponent<MeshRendererComponent>()) {
        out << YAML::Key << "MeshRenderer";
        out << YAML::BeginMap; // MeshRenderer

        MeshRendererComponent& mrc = e.getComponent<MeshRendererComponent>();

        if (mrc.MeshHandle) {
            out << YAML::Key << "Mesh" << mrc.MeshHandle->getHandle();
        }

        if (mrc.MaterialHandle) {
            out << YAML::Key << "Material" << mrc.MaterialHandle->getHandle();
        }

        out << YAML::EndMap; // MeshRenderer
    }

    // Camera
    if (e.hasComponent<CameraComponent>()) {
        out << YAML::Key << "Camera";
        out << YAML::BeginMap; // Camera

        CameraComponent& cc = e.getComponent<CameraComponent>();

        // TODO: Serialize camera

        out << YAML::EndMap; // Camera
    }

    // Rigid body
    if (e.hasComponent<RigidbodyComponent>()) {
        out << YAML::Key << "Rigidbody";
        out << YAML::BeginMap; // Rigidbody

        RigidbodyComponent& rbc = e.getComponent<RigidbodyComponent>();

        out << YAML::Key << "IsKinematic" << rbc.IsKinematic;
        out << YAML::Key << "HasGravity" << rbc.HasGravity;
        out << YAML::Key << "Mass" << rbc.Mass;

        out << YAML::EndMap; // Rigidbody
    }

    // Colliders
    if (e.hasComponent<CollidersComponent>()) {
        out << YAML::Key << "Colliders";
        CollidersComponent& cc = e.getComponent<CollidersComponent>();

        // TODO: Error check
        ::aderite::Engine::getSerializer()->writeUntrackedType(out, cc.Colliders);
    }

    // Audio sources
    if (e.hasComponent<AudioSourceComponent>()) {
        out << YAML::Key << "AudioSource";
        out << YAML::BeginSeq; // AudioSource

        AudioSourceComponent& asc = e.getComponent<AudioSourceComponent>();

        // TODO: Serialize

        out << YAML::EndSeq; // AudioSource
    }

    // Audio listener
    if (e.hasComponent<AudioListenerComponent>()) {
        out << YAML::Key << "AudioListener";
        out << YAML::BeginMap; // AudioListener

        AudioListenerComponent& alc = e.getComponent<AudioListenerComponent>();

        out << YAML::Key << "IsEnabled" << alc.IsEnabled;

        out << YAML::EndMap; // AudioListener
    }

    // Scripts
    if (e.hasComponent<ScriptsComponent>()) {
        out << YAML::Key << "Scripts";
        ScriptsComponent& sc = e.getComponent<ScriptsComponent>();

        // TODO: Error check
        ::aderite::Engine::getSerializer()->writeUntrackedType(out, sc.Scripts);
    }

    out << YAML::EndMap; // Entity
}

Entity deserialize_entity(YAML::Node& e_node, Scene* scene) {
    // Read MetaComponent info
    YAML::Node meta_node = e_node["Meta"];
    if (!meta_node) {
        LOG_ERROR("Tried to deserialize Entity without MetaComponent component, this should never happen");
        return Entity(entt::null, nullptr);
    }

    MetaComponent MetaComponent;
    MetaComponent.Name = meta_node["Name"].as<std::string>();

    // Create Entity
    Entity e = scene->createEntity(MetaComponent);

    // Deserialize rest of components

    // Transform
    auto transform_node = e_node["Transform"];
    if (transform_node) {
        auto& tc = e.addComponent<TransformComponent>();
        tc.Position = transform_node["Position"].as<glm::vec3>();
        tc.Rotation = transform_node["Rotation"].as<glm::quat>();
        tc.Scale = transform_node["Scale"].as<glm::vec3>();
    }

    // Mesh Renderer
    auto mr_node = e_node["MeshRenderer"];
    if (mr_node) {
        auto& mrc = e.addComponent<MeshRendererComponent>();

        if (mr_node["Mesh"]) {
            const io::SerializableHandle handle = mr_node["Mesh"].as<io::SerializableHandle>();
            mrc.MeshHandle = static_cast<asset::MeshAsset*>(::aderite::Engine::getSerializer()->getOrRead(handle));
            ADERITE_DYNAMIC_ASSERT(mrc.MeshHandle != nullptr, "Tried to use a deleted asset");
        }

        if (mr_node["Material"]) {
            const io::SerializableHandle handle = mr_node["Material"].as<io::SerializableHandle>();
            mrc.MaterialHandle = static_cast<asset::MaterialAsset*>(::aderite::Engine::getSerializer()->getOrRead(handle));
            ADERITE_DYNAMIC_ASSERT(mrc.MaterialHandle != nullptr, "Tried to use a deleted asset");
        }
    }

    // Camera
    auto cam_node = e_node["Camera"];
    if (cam_node) {
        auto& cc = e.addComponent<CameraComponent>();

        // TODO: Deserialize camera
    }

    // Rigid body
    auto rb_node = e_node["Rigidbody"];
    if (rb_node) {
        auto& rbc = e.addComponent<RigidbodyComponent>();

        // TODO: Error check
        rbc.IsKinematic = rb_node["IsKinematic"].as<bool>();
        rbc.HasGravity = rb_node["HasGravity"].as<bool>();
        rbc.Mass = rb_node["Mass"].as<float>();
    }

    // Colliders
    auto colliders = e_node["Colliders"];
    if (colliders) {
        if (!e.hasComponent<CollidersComponent>()) {
            e.addComponent<CollidersComponent>();
        }

        auto& cc = e.getComponent<CollidersComponent>();

        // TODO: Error check
        cc.Colliders->deserialize(::aderite::Engine::getSerializer(), colliders["Data"]);
    }

    // Audio sources
    auto audioSources = e_node["AudioSource"];
    if (audioSources) {
        auto& asc = e.addComponent<AudioSourceComponent>();

        // TODO: Deserialize
        asc.Instance = ::aderite::Engine::getAudioController()->createAudioInstance("TODO");
    }

    // Audio listener
    auto al_node = e_node["AudioListener"];
    if (al_node) {
        auto& alc = e.addComponent<AudioListenerComponent>();

        // TODO: Error check
        alc.IsEnabled = al_node["IsEnabled"].as<bool>();
    }

    // Scripts
    auto scripts = e_node["Scripts"];
    if (scripts) {
        if (!e.hasComponent<ScriptsComponent>()) {
            e.addComponent<ScriptsComponent>();
        }

        auto& sc = e.getComponent<ScriptsComponent>();

        // TODO: Error check
        sc.Scripts->deserialize(::aderite::Engine::getSerializer(), scripts["Data"]);
        sc.Scripts->pair(e);
    }

    return e;
}

Scene::~Scene() {
    auto entities = m_registry.view<scene::CollidersComponent>();
    for (auto entity : entities) {
        auto [colliders] = entities.get(entity);
        delete colliders.Colliders;
    }

    m_physicsScene->release();
}

void Scene::update(float delta) {
    auto cameraGroup = m_registry.group<scene::CameraComponent>(entt::get<scene::TransformComponent>);
    for (auto entity : cameraGroup) {
        auto [camera, transform] = cameraGroup.get(entity);

        // TODO: Update entity cameras
    }
}

void Scene::fixedUpdate(float step) {
    // Sync ECS to physics
    syncEcsToPhysics();
    m_physicsScene->simulate(step);
    m_physicsScene->fetchResults(true);
    syncPhysicsToEcs();
}

Entity Scene::createEntity(const MetaComponent& meta) {
    // TODO: Check for name conflicts
    Entity e = Entity(m_registry.create(), this);
    e.addComponent<MetaComponent>(meta).This = e.getHandle();
    return e;
}

void Scene::destroyEntity(Entity entity) {
    if (entity.hasComponent<CollidersComponent>()) {
        delete entity.getComponent<CollidersComponent>().Colliders;
    }

    if (entity.hasComponent<ScriptsComponent>()) {
        delete entity.getComponent<ScriptsComponent>().Scripts;
    }

    m_registry.destroy(entity);
}

physx::PxScene* Scene::getPhysicsScene() const {
    return m_physicsScene;
}

rendering::Pipeline* Scene::getPipeline() const {
    return m_pipeline;
}

void Scene::setPipeline(rendering::Pipeline* pipeline) {
    m_pipeline = pipeline;
}

reflection::Type Scene::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::SCENE);
}

bool Scene::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    // Entities
    emitter << YAML::Key << "Entities" << YAML::BeginSeq;

    m_registry.each([&](auto entity_id) {
        Entity e = Entity(entity_id, this);

        if (!e) {
            return;
        }

        // TODO: Error check
        serialize_entity(emitter, e);
    });

    emitter << YAML::EndSeq; // Entities

    emitter << YAML::Key << "Pipeline" << YAML::Value;
    if (m_pipeline != nullptr) {
        emitter << m_pipeline->getHandle();
    } else {
        emitter << YAML::Null;
    }

    return true;
}

bool Scene::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Entities
    auto entities = data["Entities"];
    if (entities) {
        for (auto Entity : entities) {
            // Error check
            deserialize_entity(Entity, this);
        }
    }

    if (!data["Pipeline"].IsNull()) {
        setPipeline(static_cast<rendering::Pipeline*>(
            ::aderite::Engine::getSerializer()->getOrRead(data["Pipeline"].as<io::SerializableHandle>())));
    }

    return true;
}

Scene::Scene() {
    auto physics = ::aderite::Engine::getPhysicsController()->getPhysics();

    // TODO: Configure physics properties
    physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = ::aderite::Engine::getPhysicsController()->getDispatcher();
    sceneDesc.filterShader = physics::PhysicsController::filterShader;
    sceneDesc.simulationEventCallback = this;
    // sceneDesc.flags = physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    m_physicsScene = physics->createScene(sceneDesc);
    ADERITE_DYNAMIC_ASSERT(m_physicsScene != nullptr, "Failed to create a PhysX scene");
}

template<typename T>
void Scene::onComponentAdded(Entity entity, T& component) {}

template<>
void Scene::onComponentAdded<RigidbodyComponent>(Entity entity, RigidbodyComponent& component) {
    // Can't have rigidbody without colliders
    if (!entity.hasComponent<CollidersComponent>()) {
        entity.addComponent<CollidersComponent>();
    }

    // Convert to static
    auto& colliders = entity.getComponent<CollidersComponent>();

    // Convert to dynamic
    physx::PxRigidDynamic* actor = ::aderite::Engine::getPhysicsController()->createDynamicBody();

    // Switch
    actor->userData = &entity.getComponent<MetaComponent>();
    m_physicsScene->removeActor(*colliders.Colliders->getActor());
    colliders.Colliders->setActor(actor);
    m_physicsScene->addActor(*actor);
}

template<>
void Scene::onComponentAdded<CollidersComponent>(Entity entity, CollidersComponent& component) {
    component.Colliders = new physics::ColliderList();
    physx::PxRigidStatic* actor = ::aderite::Engine::getPhysicsController()->createStaticBody();
    actor->userData = &entity.getComponent<MetaComponent>();
    component.Colliders->setActor(actor);
    m_physicsScene->addActor(*actor);
}

template<>
void Scene::onComponentAdded<ScriptsComponent>(Entity entity, ScriptsComponent& component) {
    component.Scripts = new scripting::ScriptList();
}

template<>
void Scene::onComponentAdded<AudioSourceComponent>(Entity entity, AudioSourceComponent& component) {
    // Add transform if don't have already
    if (!entity.hasComponent<TransformComponent>()) {
        entity.addComponent<TransformComponent>();
    }
}

template<>
void Scene::onComponentAdded<AudioListenerComponent>(Entity entity, AudioListenerComponent& component) {
    // Add transform if don't have already
    if (!entity.hasComponent<TransformComponent>()) {
        entity.addComponent<TransformComponent>();
    }
}

template<>
void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
    // Add transform if don't have already
    if (!entity.hasComponent<TransformComponent>()) {
        entity.addComponent<TransformComponent>();
    }
}

template<typename T>
void Scene::onComponentRemoved(Entity entity, T& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, TransformComponent& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, MeshRendererComponent& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, AudioSourceComponent& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, AudioListenerComponent& component) {}

template<>
void Scene::onComponentRemoved<RigidbodyComponent>(Entity entity, RigidbodyComponent& component) {
    // Convert to static
    auto& colliders = entity.getComponent<CollidersComponent>();

    // Make static
    physx::PxRigidStatic* actor = ::aderite::Engine::getPhysicsController()->createStaticBody();

    // Switch
    m_physicsScene->removeActor(*colliders.Colliders->getActor());
    colliders.Colliders->setActor(actor);
    m_physicsScene->addActor(*actor);
}

template<>
void Scene::onComponentRemoved<CollidersComponent>(Entity entity, CollidersComponent& component) {
    if (entity.hasComponent<RigidbodyComponent>()) {
        entity.removeComponent<RigidbodyComponent>();
    }
    m_physicsScene->removeActor(*component.Colliders->getActor());
    delete component.Colliders;
}

template<>
void Scene::onComponentRemoved<ScriptsComponent>(Entity entity, ScriptsComponent& component) {
    delete component.Scripts;
}

void Scene::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {
    for (physx::PxU32 i = 0; i < nbPairs; i++) {
        const physx::PxContactPair& cp = pairs[i];

        physx::PxRigidActor* actor1 = pairHeader.actors[0];
        physx::PxRigidActor* actor2 = pairHeader.actors[1];

        MetaComponent* e1 = static_cast<MetaComponent*>(actor1->userData);
        MetaComponent* e2 = static_cast<MetaComponent*>(actor2->userData);

        Entity a1Entity = Entity(e1->This, this);
        Entity a2Entity = Entity(e2->This, this);
        if (a1Entity.hasComponent<ScriptsComponent>()) {
            ScriptsComponent& scriptsActor = a1Entity.getComponent<ScriptsComponent>();

            if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                scriptsActor.Scripts->onCollisionEnter(a2Entity);
            } else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
                scriptsActor.Scripts->onCollisionLeave(a2Entity);
            }
        }

        if (a2Entity.hasComponent<ScriptsComponent>()) {
            ScriptsComponent& scriptsActor = a2Entity.getComponent<ScriptsComponent>();

            if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                scriptsActor.Scripts->onCollisionEnter(a1Entity);
            } else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
                scriptsActor.Scripts->onCollisionLeave(a1Entity);
            }
        }
    }
}

void Scene::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs) {
    for (physx::PxU32 i = 0; i < nbPairs; i++) {
        const physx::PxTriggerPair& cp = pairs[i];

        physx::PxRigidActor* actor = cp.otherActor;
        physx::PxRigidActor* trigger = cp.triggerActor;

        MetaComponent* metaActor = static_cast<MetaComponent*>(actor->userData);
        MetaComponent* metaTrigger = static_cast<MetaComponent*>(trigger->userData);

        Entity actorEntity = Entity(metaActor->This, this);
        Entity triggerEntity = Entity(metaTrigger->This, this);
        if (actorEntity.hasComponent<ScriptsComponent>()) {
            ScriptsComponent& scriptsActor = actorEntity.getComponent<ScriptsComponent>();

            if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                scriptsActor.Scripts->onTriggerEnter(triggerEntity);
            } else if (cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
                scriptsActor.Scripts->onTriggerLeave(triggerEntity);
            }
        }
    }
}

void Scene::syncActorToEcs(physx::PxRigidActor* actor, const scene::CollidersComponent& colliders,
                           const scene::TransformComponent& transform) {
    // Transform
    actor->setGlobalPose(
        physx::PxTransform(physx::PxVec3 {transform.Position.x, transform.Position.y, transform.Position.z},
                           physx::PxQuat {transform.Rotation.x, transform.Rotation.y, transform.Rotation.z, transform.Rotation.w}));

    // Scale
    colliders.Colliders->setScale(transform.Scale);

    // Collision group
    physx::PxSetGroup(*actor, 0);
}

void Scene::syncEcsToPhysics() {
    // Sync changes with ECS
    auto dynamicGroup = m_registry.group<scene::RigidbodyComponent>(entt::get<scene::CollidersComponent, scene::TransformComponent>);
    for (auto entity : dynamicGroup) {
        auto [rigidbody, colliders, transform] =
            dynamicGroup.get<scene::RigidbodyComponent, scene::CollidersComponent, scene::TransformComponent>(entity);

        physx::PxRigidDynamic* actor = static_cast<physx::PxRigidDynamic*>(colliders.Colliders->getActor());
        if (actor != nullptr && (transform.WasAltered || rigidbody.WasAltered)) {
            // Sync ECS -> PhysX
            syncActorToEcs(actor, colliders, transform);

            // Settings
            actor->setMass(rigidbody.Mass);
            actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !rigidbody.HasGravity);
            actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, rigidbody.IsKinematic);

            // Reset flags
            transform.WasAltered = false;
            rigidbody.WasAltered = false;
        }
    }

    auto staticGroup =
        m_registry.group<scene::CollidersComponent>(entt::get<scene::TransformComponent>, entt::exclude<scene::RigidbodyComponent>);
    for (auto entity : staticGroup) {
        auto [colliders, transform] = staticGroup.get<scene::CollidersComponent, scene::TransformComponent>(entity);

        physx::PxRigidStatic* actor = static_cast<physx::PxRigidStatic*>(colliders.Colliders->getActor());
        if (actor != nullptr && transform.WasAltered) {
            LOG_WARN("Altered position of static physics entity");

            // Sync ECS -> PhysX
            syncActorToEcs(actor, colliders, transform);

            // Reset flags
            transform.WasAltered = false;
        }
    }
}

void Scene::syncPhysicsToEcs() {
    // Sync changes with physics
    auto physicsGroup = m_registry.group<scene::CollidersComponent>(entt::get<scene::TransformComponent>);
    for (auto entity : physicsGroup) {
        auto [colliders, transform] = physicsGroup.get<scene::CollidersComponent, scene::TransformComponent>(entity);

        physx::PxRigidActor* actor = static_cast<physx::PxRigidActor*>(colliders.Colliders->getActor());

        // Transform
        physx::PxTransform pxt = actor->getGlobalPose();
        transform.Position = {pxt.p.x, pxt.p.y, pxt.p.z};
        transform.Rotation = {pxt.q.w, pxt.q.x, pxt.q.y, pxt.q.z};
    }
}

} // namespace scene
} // namespace aderite
