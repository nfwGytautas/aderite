#include "Scene.hpp"

#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/EntitySelector.hpp"
#include "aderite/scene/SceneSerializer.hpp"
#include "aderite/scene/Transform.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scene {

Scene::~Scene() {
    delete m_physics;

    // TODO: Free all the variables created
}

void Scene::update(float delta) {
    for (EntitySelector* selector : m_entitySelectors) {
        // Regenerate selector views
        selector->regenerate();
    }

    // auto cameraGroup = m_registry.group<scene::CameraComponent>(entt::get<scene::TransformComponent>);
    // for (auto entity : cameraGroup) {
    //    auto [camera, transform] = cameraGroup.get(entity);

    //    // TODO: Update entity cameras
    //}
}

void Scene::addSource(audio::AudioSource* source) {
    m_audioSources.push_back(source);
}

void Scene::addScriptSystem(scripting::ScriptSystem* system) {
    ADERITE_DYNAMIC_ASSERT(system != nullptr, "Passed nullptr scripting::ScriptSystem to addScriptSystem");

    // Check if a system with the same name exists
    auto it = std::find_if(m_systems.begin(), m_systems.end(), [system](const scripting::ScriptSystem* sys) {
        return sys->getName() == system->getName();
    });

    if (it != m_systems.end()) {
        LOG_ERROR("Tried to add a system that already exists in the scene {0}", system->getName());
        return;
    }

    system->attachToScene(this);
    m_systems.push_back(system);
}

void Scene::addEntitySelector(EntitySelector* selector) {
    m_entitySelectors.push_back(selector);
    selector->setScene(this);
}

size_t Scene::getFreeTagSlots() const {
    size_t count = 0;
    for (const std::string& tag : m_tags) {
        if (tag.empty()) {
            count++;
        }
    }

    return count;
}

size_t Scene::getTagIndex(const std::string& name) const {
    for (size_t i = 0; i < m_tags.size(); i++) {
        if (m_tags[i] == name) {
            return i + 1;
        }
    }

    return c_MaxTags + 2;
}

void Scene::addTag(const std::string& name) {
    ADERITE_DYNAMIC_ASSERT(this->getFreeTagSlots() > 0, "No more tags can be inserted into the scene tag list");

    // Check if a tag with the same name already exists
    auto it = std::find(m_tags.begin(), m_tags.end(), name);
    if (it != m_tags.end()) {
        LOG_WARN("Tried to add a tag {0} that already exists", name);
        return;
    }

    // Add tag
    auto it2 = std::find(m_tags.begin(), m_tags.end(), "");
    *it2 = name;
}

void Scene::removeTag(const std::string& name) {
    auto it = std::find(m_tags.begin(), m_tags.end(), name);
    if (it == m_tags.end()) {
        LOG_WARN("Tried to remove tag {0} that the scene doesn't have", name);
        return;
    }

    *it = "";
}

rendering::Pipeline* Scene::getPipeline() const {
    return m_pipeline;
}

physics::PhysicsScene* Scene::getPhysicsScene() const {
    return m_physics;
}

EntitySelector* Scene::getSelector(const std::string& name) const {
    auto it = std::find_if(m_entitySelectors.begin(), m_entitySelectors.end(), [name](const EntitySelector* selector) {
        return selector->getName() == name;
    });

    if (it == m_entitySelectors.end()) {
        return nullptr;
    }

    return *it;
}

void Scene::setPipeline(rendering::Pipeline* pipeline) {
    m_pipeline = pipeline;
}

void Scene::addEntity(Entity* entity) {
    entity->setScene(this);
    m_entities.push_back(entity);

    for (EntitySelector* selector : m_entitySelectors) {
        selector->onEntityAdded(entity);
    }
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
    m_tags.resize(c_MaxTags, "");
}

} // namespace scene
} // namespace aderite
