#include "Scene.hpp"

#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/scene/DynamicPhysicsRegion.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Scenery.hpp"
#include "aderite/scene/StaticPhysicsRegion.hpp"
#include "aderite/scene/Visual.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

namespace aderite {
namespace scene {

#define SERIALIZE_LIST(rootName, objects)               \
    emitter << YAML::Key << rootName << YAML::BeginSeq; \
    for (const auto* object : objects) {                \
        serializer->writeObject(emitter, object);       \
    }                                                   \
    emitter << YAML::EndSeq;

// TODO: Check for failed parsing
#define DESERIALIZE_LIST(rootName, type)                                            \
    {                                                                               \
        auto objects = data[rootName];                                              \
        if (objects) {                                                              \
            for (auto object : objects) {                                           \
                type* parsed = static_cast<type*>(serializer->parseObject(object)); \
                this->add(parsed);                                                  \
            }                                                                       \
        }                                                                           \
    }

template<typename T>
bool addObject(std::vector<T*>& list, T* object) {
    ADERITE_DYNAMIC_ASSERT(object != nullptr, "Passed nullptr to addObject");

    // Check if a object with the same name exists
    auto it = std::find_if(list.begin(), list.end(), [object](const T* obj) {
        return obj->getName() == object->getName();
    });

    if (it != list.end()) {
        LOG_WARN("[Scene] Tried to add a object that already exists in the scene {0}", object->getName());
        return false;
    }

    list.push_back(object);
    return true;
}

template<typename T>
void removeObject(std::vector<T*>& list, T* object) {
    ADERITE_DYNAMIC_ASSERT(object != nullptr, "Tried to remove nullptr object");
    auto it = std::find_if(list.begin(), list.end(), [object](const T* obj) {
        return object->getName() == obj->getName();
    });
    ADERITE_DYNAMIC_ASSERT(it != list.end(), "Tried to remove entity that doesn't exist in the scene");
    list.erase(it);
    delete object;
}

Scene::~Scene() {
    LOG_TRACE("[Scene] Deleting scene {0}", this->getName());

    // Objects
    for (Visual* visual : m_visuals) {
        delete visual;
    }
    for (Scenery* scenery : m_scenery) {
        delete scenery;
    }
    for (Entity* entity : m_entities) {
        delete entity;
    }

    // Physics
    for (StaticPhysicsRegion* region : m_staticPhysicsRegions) {
        delete region;
    }
    for (DynamicPhysicsRegion* region : m_dynamicPhysicsRegions) {
        delete region;
    }

    // Audio
    for (audio::AudioSource* source : m_audioSources) {
        delete source;
    }
    for (audio::AudioListener* listener : m_audioListeners) {
        delete listener;
    }

    LOG_INFO("[Scene] Scene {0} deleted", this->getName());
}

void Scene::update(float delta) {}

void Scene::add(Visual* visual) {
    addObject(m_visuals, visual);
}

void Scene::remove(Visual* visual) {
    removeObject(m_visuals, visual);
}

const std::vector<Visual*> Scene::getVisuals() const {
    return m_visuals;
}

void Scene::add(Scenery* scenery) {
    addObject(m_scenery, scenery);
    this->addActor(scenery);
}

void Scene::remove(Scenery* scenery) {
    removeObject(m_scenery, scenery);
}

const std::vector<Scenery*> Scene::getScenery() const {
    return m_scenery;
}

void Scene::add(Entity* entity) {
    addObject(m_entities, entity);
    this->addActor(entity);
}

void Scene::remove(Entity* entity) {
    removeObject(m_entities, entity);
}

const std::vector<Entity*> Scene::getEntities() const {
    return m_entities;
}

void Scene::add(StaticPhysicsRegion* region) {
    addObject(m_staticPhysicsRegions, region);
    this->addActor(region);
}

void Scene::remove(StaticPhysicsRegion* region) {
    removeObject(m_staticPhysicsRegions, region);
}

const std::vector<StaticPhysicsRegion*> Scene::getStaticPhysicsRegions() const {
    return m_staticPhysicsRegions;
}

void Scene::add(DynamicPhysicsRegion* region) {
    addObject(m_dynamicPhysicsRegions, region);
    this->addActor(region);
}

void Scene::remove(DynamicPhysicsRegion* region) {
    removeObject(m_dynamicPhysicsRegions, region);
}

const std::vector<DynamicPhysicsRegion*> Scene::getDynamicPhysicsRegions() const {
    return m_dynamicPhysicsRegions;
}

void Scene::add(audio::AudioListener* listener) {
    addObject(m_audioListeners, listener);
}

void Scene::remove(audio::AudioListener* listener) {
    removeObject(m_audioListeners, listener);
}

const std::vector<audio::AudioListener*>& Scene::getAudioListeners() const {
    return m_audioListeners;
}

void Scene::add(audio::AudioSource* source) {
    addObject(m_audioSources, source);
}

void Scene::remove(audio::AudioSource* source) {
    removeObject(m_audioSources, source);
}

const std::vector<audio::AudioSource*>& Scene::getAudioSources() const {
    return m_audioSources;
}

void Scene::add(Camera* camera) {
    addObject(m_cameras, camera);
}

void Scene::remove(Camera* camera) {
    removeObject(m_cameras, camera);
}

const std::vector<Camera*>& Scene::getCameras() const {
    return m_cameras;
}

audio::AudioSource* Scene::getSource(const std::string& name) const {
    auto it = std::find_if(m_audioSources.begin(), m_audioSources.end(), [name](const audio::AudioSource* source) {
        return source->getName() == name;
    });

    if (it == m_audioSources.end()) {
        LOG_WARN("[Scene] Tried to get audio source {0} from scene {1}, but the scene doesn't have it", name, this->getName());
        return nullptr;
    }

    return *it;
}

reflection::Type Scene::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::SCENE);
}

bool Scene::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!PhysicsScene::serialize(serializer, emitter)) {
        return false;
    }

    if (!ScriptEventMap::serialize(serializer, emitter)) {
        return false;
    }

    // Object
    SERIALIZE_LIST("Visuals", m_visuals);
    SERIALIZE_LIST("Scenery", m_scenery);
    SERIALIZE_LIST("Entities", m_entities);

    // Physics
    SERIALIZE_LIST("StaticPhysicsRegions", m_staticPhysicsRegions);
    SERIALIZE_LIST("DynamicPhysicsRegions", m_dynamicPhysicsRegions);

    // Audio
    SERIALIZE_LIST("AudioListeners", m_audioListeners);
    SERIALIZE_LIST("AudioSources", m_audioSources);

    // Other
    SERIALIZE_LIST("Cameras", m_cameras);
}

bool Scene::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!PhysicsScene::deserialize(serializer, data)) {
        return false;
    }

    if (!ScriptEventMap::deserialize(serializer, data)) {
        return false;
    }

    // Object
    DESERIALIZE_LIST("Visuals", Visual);
    DESERIALIZE_LIST("Scenery", Scenery);
    DESERIALIZE_LIST("Entities", Entity);

    // Physics
    DESERIALIZE_LIST("StaticPhysicsRegions", StaticPhysicsRegion);
    DESERIALIZE_LIST("DynamicPhysicsRegions", DynamicPhysicsRegion);

    // Audio
    DESERIALIZE_LIST("AudioListeners", audio::AudioListener);
    DESERIALIZE_LIST("AudioSources", audio::AudioSource);

    // Other
    DESERIALIZE_LIST("Cameras", Camera);
}

Scene::Scene() {}

} // namespace scene
} // namespace aderite
