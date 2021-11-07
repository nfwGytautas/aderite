#include "Scene.hpp"

#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/SceneSerializer.hpp"
#include "aderite/scene/Transform.hpp"
#include "aderite/scripting/ScriptList.hpp"

namespace aderite {
namespace scene {

Scene::~Scene() {
    delete m_physics;
}

void Scene::update(float delta) {
    // auto cameraGroup = m_registry.group<scene::CameraComponent>(entt::get<scene::TransformComponent>);
    // for (auto entity : cameraGroup) {
    //    auto [camera, transform] = cameraGroup.get(entity);

    //    // TODO: Update entity cameras
    //}
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

void Scene::addEntity(Entity* entity) {
    entity->setScene(this);
    m_entities.push_back(entity);
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

} // namespace scene
} // namespace aderite
