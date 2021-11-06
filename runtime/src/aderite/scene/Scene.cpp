#include "Scene.hpp"

#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/SceneSerializer.hpp"
#include "aderite/scene/components/Actors.hpp"
#include "aderite/scene/components/Audio.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/scene/components/Transform.hpp"
#include "aderite/scripting/ScriptList.hpp"

namespace aderite {
namespace scene {

Scene::~Scene() {
    delete m_physics;
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
    if (entity.hasComponent<ScriptsComponent>()) {
        delete entity.getComponent<ScriptsComponent>().Scripts;
    }

    m_registry.destroy(entity);
}

void Scene::addSource(audio::AudioSource* source) {
    m_audioSources.push_back(source);
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

bool Scene::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    SceneSerializer ss;
    return ss.serialize(this, serializer, emitter);
}

bool Scene::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    SceneSerializer ss;
    return ss.deserialize(this, serializer, data);
}

Scene::Scene() {
    m_physics = new physics::PhysicsScene();
}

template<typename T>
void Scene::onComponentAdded(Entity entity, T& component) {}

template<>
void Scene::onComponentAdded(Entity entity, MeshRendererComponent& component) {}

template<>
void Scene::onComponentAdded<CollidersComponent>(Entity entity, CollidersComponent& component) {}

template<>
void Scene::onComponentAdded<ScriptsComponent>(Entity entity, ScriptsComponent& component) {
    component.Scripts = new scripting::ScriptList();
}

template<>
void Scene::onComponentAdded<AudioListenerComponent>(Entity entity, AudioListenerComponent& component) {
    // Add transform if don't have already
    if (!entity.hasComponent<TransformComponent>()) {
        entity.addComponent<TransformComponent>();
    }
}

template<>
void Scene::onComponentAdded<DynamicActor>(Entity entity, DynamicActor& component) {
    // Add transform if don't have already
    if (!entity.hasComponent<TransformComponent>()) {
        entity.addComponent<TransformComponent>();
    }

    component.Actor =
        static_cast<physics::DynamicActor*>(this->m_physics->createDynamicBody(entity, entity.getComponent<TransformComponent>()));
}

template<>
void Scene::onComponentAdded<StaticActor>(Entity entity, StaticActor& component) {
    // Add transform if don't have already
    if (!entity.hasComponent<TransformComponent>()) {
        entity.addComponent<TransformComponent>();
    }

    component.Actor =
        static_cast<physics::StaticActor*>(this->m_physics->createStaticBody(entity, entity.getComponent<TransformComponent>()));
}

template<>
void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
    // Add transform if don't have already
    if (!entity.hasComponent<TransformComponent>()) {
        entity.addComponent<TransformComponent>();
    }
}

template<>
void Scene::onComponentAdded<PhysicsCallbackComponent>(Entity entity, PhysicsCallbackComponent& component) {}

template<typename T>
void Scene::onComponentRemoved(Entity entity, T& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, TransformComponent& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, MeshRendererComponent& component) {}

template<>
void Scene::onComponentRemoved(Entity entity, AudioListenerComponent& component) {}

template<>
void Scene::onComponentRemoved<DynamicActor>(Entity entity, DynamicActor& component) {
    this->m_physics->detachActor(component.Actor);
}

template<>
void Scene::onComponentRemoved<StaticActor>(Entity entity, StaticActor& component) {
    this->m_physics->detachActor(component.Actor);
}

template<>
void Scene::onComponentRemoved<CollidersComponent>(Entity entity, CollidersComponent& component) {}

template<>
void Scene::onComponentRemoved<ScriptsComponent>(Entity entity, ScriptsComponent& component) {
    delete component.Scripts;
}

} // namespace scene
} // namespace aderite
