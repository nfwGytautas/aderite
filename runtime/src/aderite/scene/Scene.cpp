#include "Scene.hpp"

#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/EntitySelector.hpp"
#include "aderite/scene/SceneSerializer.hpp"
#include "aderite/scene/Transform.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scene {

Scene::~Scene() {
    LOG_TRACE("[Scene] Deleting scene {0}", this->getHandle());

    // Free resources
    delete m_physics;

    for (audio::AudioSource* source : m_audioSources) {
        delete source;
    }

    for (audio::AudioListener* listener : m_audioListeners) {
        delete listener;
    }

    for (Entity* entity : m_entities) {
        delete entity;
    }

    for (scripting::ScriptSystem* system : m_systems) {
        delete system;
    }

    for (EntitySelector* selector : m_entitySelectors) {
        delete selector;
    }

    LOG_INFO("[Scene] Scene {0} deleted", this->getHandle());
}

void Scene::update(float delta) {
    for (EntitySelector* selector : m_entitySelectors) {
        // Regenerate selector views
        selector->regenerate();
    }
}

void Scene::addScriptSystem(scripting::ScriptSystem* system) {
    LOG_TRACE("[Scene] Adding system {0} to scene {1}", system->getName(), this->getHandle());
    ADERITE_DYNAMIC_ASSERT(system != nullptr, "Passed nullptr scripting::ScriptSystem to addScriptSystem");

    // Check if a system with the same name exists
    auto it = std::find_if(m_systems.begin(), m_systems.end(), [system](const scripting::ScriptSystem* sys) {
        return sys->getName() == system->getName();
    });

    if (it != m_systems.end()) {
        LOG_ERROR("[Scene] Tried to add a system that already exists in the scene {0}", system->getName());
        return;
    }

    system->attachToScene(this);
    m_systems.push_back(system);
}

void Scene::addEntitySelector(EntitySelector* selector) {
    LOG_TRACE("[Scene] Adding entity selector {0} to scene {1}", selector->getName(), this->getHandle());
    m_entitySelectors.push_back(selector);
    selector->setScene(this);
}

void Scene::addAudioListener(audio::AudioListener* listener) {
    LOG_TRACE("[Scene] Adding audio listener {0} to scene {1}", listener->getName(), this->getHandle());
    m_audioListeners.push_back(listener);
}

void Scene::addAudioSource(audio::AudioSource* source) {
    LOG_TRACE("[Scene] Adding audio source {0} to scene {1}", source->getName(), this->getHandle());
    m_audioSources.push_back(source);
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
    LOG_TRACE("[Scene] Adding tag {0} to scene {1}", name, this->getHandle());
    ADERITE_DYNAMIC_ASSERT(this->getFreeTagSlots() > 0, "No more tags can be inserted into the scene tag list");

    // Check if a tag with the same name already exists
    auto it = std::find(m_tags.begin(), m_tags.end(), name);
    if (it != m_tags.end()) {
        LOG_WARN("[Scene] Tried to add a tag {0} that already exists", name);
        return;
    }

    // Add tag
    auto it2 = std::find(m_tags.begin(), m_tags.end(), "");
    *it2 = name;
}

void Scene::removeTag(const std::string& name) {
    auto it = std::find(m_tags.begin(), m_tags.end(), name);
    if (it == m_tags.end()) {
        LOG_WARN("[Scene] Tried to remove tag {0} that the scene {1} doesn't have", name, this->getHandle());
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
        LOG_WARN("[Scene] Tried to get selector {0} from scene {1}, but the scene doesn't have it", name, this->getHandle());
        return nullptr;
    }

    return *it;
}

audio::AudioSource* Scene::getSource(const std::string& name) const {
    auto it = std::find_if(m_audioSources.begin(), m_audioSources.end(), [name](const audio::AudioSource* source) {
        return source->getName() == name;
    });

    if (it == m_audioSources.end()) {
        LOG_WARN("[Scene] Tried to get audio source {0} from scene {1}, but the scene doesn't have it", name, this->getHandle());
        return nullptr;
    }

    return *it;
}

void Scene::setPipeline(rendering::Pipeline* pipeline) {
    LOG_WARN("[Scene] Setting scene {0} pipeline to {1}", this->getHandle(), pipeline->getHandle());
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
