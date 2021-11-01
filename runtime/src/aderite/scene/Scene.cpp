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
#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/components/Actors.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/scene/components/Meta.hpp"
#include "aderite/scene/components/Transform.hpp"
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

    // Dynamic actor
    if (e.hasComponent<DynamicActor>()) {
        out << YAML::Key << "DynamicActor";
        out << YAML::BeginMap; // DynamicActor

        DynamicActor& dac = e.getComponent<DynamicActor>();

        ::aderite::Engine::getSerializer()->writeUntrackedType(out, dac.Actor);

        out << YAML::EndMap; // DynamicActor
    }

    // Static actor
    if (e.hasComponent<StaticActor>()) {
        out << YAML::Key << "StaticActor";
        out << YAML::BeginMap; // StaticActor

        StaticActor& dac = e.getComponent<StaticActor>();

        ::aderite::Engine::getSerializer()->writeUntrackedType(out, dac.Actor);

        out << YAML::EndMap; // StaticActor
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

    // Dynamic actor
    auto da_node = e_node["DynamicActor"];
    if (da_node) {
        auto& dac = e.addComponent<DynamicActor>();

        // TODO: Error check
        dac.Actor = static_cast<physics::DynamicActor*>(::aderite::Engine::getSerializer()->parseUntrackedType(da_node));
    }

    // Static actor
    auto sa_node = e_node["StaticActor"];
    if (sa_node) {
        auto& sac = e.addComponent<StaticActor>();

        // TODO: Error check
        sac.Actor = static_cast<physics::StaticActor*>(::aderite::Engine::getSerializer()->parseUntrackedType(sa_node));
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
}

void Scene::update(float delta) {
    auto cameraGroup = m_registry.group<scene::CameraComponent>(entt::get<scene::TransformComponent>);
    for (auto entity : cameraGroup) {
        auto [camera, transform] = cameraGroup.get(entity);

        // TODO: Update entity cameras
    }
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

rendering::Pipeline* Scene::getPipeline() const {
    return m_pipeline;
}

physics::PhysicsScene* Scene::getPhysicsScene() const {
    return m_physics;
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
    m_physics = new physics::PhysicsScene();
}

template<typename T>
void Scene::onComponentAdded(Entity entity, T& component) {}

template<>
void Scene::onComponentAdded<CollidersComponent>(Entity entity, CollidersComponent& component) {
    component.Colliders = new physics::ColliderList();
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
void Scene::onComponentRemoved<DynamicActor>(Entity entity, DynamicActor& component) {
    component.Actor->detach();
}

template<>
void Scene::onComponentRemoved<StaticActor>(Entity entity, StaticActor& component) {
    component.Actor->detach();
}

template<>
void Scene::onComponentRemoved<CollidersComponent>(Entity entity, CollidersComponent& component) {
    delete component.Colliders;
}

template<>
void Scene::onComponentRemoved<ScriptsComponent>(Entity entity, ScriptsComponent& component) {
    delete component.Scripts;
}

} // namespace scene
} // namespace aderite
