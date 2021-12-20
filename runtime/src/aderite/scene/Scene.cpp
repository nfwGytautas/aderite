#include "Scene.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Scenery.hpp"
#include "aderite/scene/Visual.hpp"
#include "aderite/scripting/ScriptClass.hpp"
#include "aderite/scripting/ScriptData.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

namespace aderite {
namespace scene {

#define SERIALIZE_LIST(rootName, objects)               \
    emitter << YAML::Key << rootName << YAML::BeginSeq; \
    for (const auto& object : objects) {                \
        serializer->writeObject(emitter, object.get()); \
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
bool addObject(std::vector<std::unique_ptr<T>>& list, T* object) {
    ADERITE_DYNAMIC_ASSERT(object != nullptr, "Passed nullptr to addObject");

    // Check if a object with the same name exists
    auto it = std::find_if(list.begin(), list.end(), [&object](const std::unique_ptr<T>& obj) {
        return obj->getName() == object->getName();
    });

    if (it != list.end()) {
        LOG_WARN("[Scene] Tried to add a object that already exists in the scene {0}", object->getName());
        return false;
    }

    list.push_back(std::unique_ptr<T>(object));
    return true;
}

template<typename T>
void removeObject(std::vector<std::unique_ptr<T>>& list, T* object) {
    ADERITE_DYNAMIC_ASSERT(object != nullptr, "Tried to remove nullptr object");
    auto it = std::find_if(list.begin(), list.end(), [object](const std::unique_ptr<T>& obj) {
        return object->getName() == obj->getName();
    });
    ADERITE_DYNAMIC_ASSERT(it != list.end(), "Tried to remove entity that doesn't exist in the scene");
    list.erase(it);
}

Scene::~Scene() {
    LOG_TRACE("[Scene] Deleting scene {0}", this->getName());

    // Objects
    m_visuals.clear();
    m_scenery.clear();
    m_entities.clear();

    // Audio
    m_audioSources.clear();
    m_audioListeners.clear();

    // Other
    m_cameras.clear();
    m_scriptData.clear();

    LOG_INFO("[Scene] Scene {0} deleted", this->getName());
}

void Scene::update(float delta) {
    // Free marked entities
    m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(),
                                    [](const std::unique_ptr<Entity>& e) {
                                        return e->isMarkedForDeletion();
                                    }),
                     m_entities.end());
}

void Scene::add(Visual* visual) {
    addObject(m_visuals, visual);
}

void Scene::remove(Visual* visual) {
    removeObject(m_visuals, visual);
}

const std::vector<std::unique_ptr<Visual>>& Scene::getVisuals() const {
    return m_visuals;
}

void Scene::add(Scenery* scenery) {
    addObject(m_scenery, scenery);
    this->addActor(scenery);
}

void Scene::remove(Scenery* scenery) {
    removeObject(m_scenery, scenery);
}

const std::vector<std::unique_ptr<Scenery>>& Scene::getScenery() const {
    return m_scenery;
}

void Scene::add(Entity* entity) {
    addObject(m_entities, entity);
    this->addActor(entity);
}

void Scene::remove(Entity* entity) {
    removeObject(m_entities, entity);
}

const std::vector<std::unique_ptr<Entity>>& Scene::getEntities() const {
    return m_entities;
}

void Scene::add(audio::AudioListener* listener) {
    addObject(m_audioListeners, listener);
}

void Scene::remove(audio::AudioListener* listener) {
    removeObject(m_audioListeners, listener);
}

const std::vector<std::unique_ptr<audio::AudioListener>>& Scene::getAudioListeners() const {
    return m_audioListeners;
}

void Scene::add(audio::AudioSource* source) {
    addObject(m_audioSources, source);
}

void Scene::remove(audio::AudioSource* source) {
    removeObject(m_audioSources, source);
}

const std::vector<std::unique_ptr<audio::AudioSource>>& Scene::getAudioSources() const {
    return m_audioSources;
}

void Scene::add(Camera* camera) {
    addObject(m_cameras, camera);
}

void Scene::remove(Camera* camera) {
    removeObject(m_cameras, camera);
}

const std::vector<std::unique_ptr<Camera>>& Scene::getCameras() const {
    return m_cameras;
}

void Scene::updateScriptDataEntries() {
    auto scripts = ::aderite::Engine::getScriptManager()->getScripts();

    // Remove unused ones
    for (auto it = m_scriptData.begin(); it != m_scriptData.end();) {
        auto itScripts = std::find_if(scripts.begin(), scripts.end(), [&](const scripting::ScriptClass* sc) {
            return (*it)->getScriptName() == sc->getName();
        });

        if (itScripts == scripts.end()) {
            it = m_scriptData.erase(it);
        } else {
            ++it;
        }
    }

    // Add new ones
    for (scripting::ScriptClass* script : scripts) {
        auto it = std::find_if(m_scriptData.begin(), m_scriptData.end(), [&](const std::unique_ptr<scripting::ScriptData>& sd) {
            return sd->getScriptName() == script->getName();
        });

        if (it == m_scriptData.end()) {
            scripting::ScriptData* sd = new scripting::ScriptData(this);
            sd->setScriptName(script->getName());
            m_scriptData.push_back(std::unique_ptr<scripting::ScriptData>(sd));
        }
    }
}

const std::vector<std::unique_ptr<scripting::ScriptData>>& Scene::getScriptData() const {
    return m_scriptData;
}

audio::AudioSource* Scene::getSource(const std::string& name) const {
    auto it = std::find_if(m_audioSources.begin(), m_audioSources.end(), [name](const std::unique_ptr<audio::AudioSource>& source) {
        return source->getName() == name;
    });

    if (it == m_audioSources.end()) {
        LOG_WARN("[Scene] Tried to get audio source {0} from scene {1}, but the scene doesn't have it", name, this->getName());
        return nullptr;
    }

    return it->get();
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

    // Audio
    SERIALIZE_LIST("AudioListeners", m_audioListeners);
    SERIALIZE_LIST("AudioSources", m_audioSources);

    // Other
    SERIALIZE_LIST("Cameras", m_cameras);

    // Script data
    emitter << YAML::Key << "Scripts" << YAML::BeginSeq;
    for (const std::unique_ptr<scripting::ScriptData>& sd : m_scriptData) {
        sd->serialize(serializer, emitter);
    }
    emitter << YAML::EndSeq;
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

    // Audio
    DESERIALIZE_LIST("AudioListeners", audio::AudioListener);
    DESERIALIZE_LIST("AudioSources", audio::AudioSource);

    // Other
    DESERIALIZE_LIST("Cameras", Camera);

    // Script data
    auto scripts = data["Scripts"];
    if (scripts) {
        for (const YAML::Node& node : scripts) {
            scripting::ScriptData* sd = new scripting::ScriptData(this);
            sd->deserialize(serializer, node);
            m_scriptData.push_back(std::unique_ptr<scripting::ScriptData>(sd));
        }
    }
}

Scene::Scene() {}

} // namespace scene
} // namespace aderite
