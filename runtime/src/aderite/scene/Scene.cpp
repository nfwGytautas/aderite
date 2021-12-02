#include "Scene.hpp"

#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/scene/SceneSerializer.hpp"
#include "aderite/scene/Visual.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

namespace aderite {
namespace scene {

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
    LOG_TRACE("[Scene] Deleting scene {0}", this->getHandle());

    // Free resources
    delete m_physics;

    for (Visual* visual : m_visuals) {
        delete visual;
    }

    for (audio::AudioSource* source : m_audioSources) {
        delete source;
    }

    for (audio::AudioListener* listener : m_audioListeners) {
        delete listener;
    }

    for (scripting::ScriptSystem* system : m_systems) {
        delete system;
    }

    LOG_INFO("[Scene] Scene {0} deleted", this->getHandle());
}

void Scene::update(float delta) {}

void Scene::addVisual(Visual* visual) {
    addObject(m_visuals, visual);
}

void Scene::removeVisual(Visual* visual) {
    removeObject(m_visuals, visual);
}

const std::vector<Visual*> Scene::getVisuals() const {
    return m_visuals;
}

void Scene::addScenery(Scenery* scenery) {
    addObject(m_scenery, scenery);
}

void Scene::removeScenery(Scenery* scenery) {
    removeObject(m_scenery, scenery);
}

const std::vector<Scenery*> Scene::getScenery() const {
    return m_scenery;
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

void Scene::addAudioListener(audio::AudioListener* listener) {
    addObject(m_audioListeners, listener);
}

void Scene::addAudioSource(audio::AudioSource* source) {
    addObject(m_audioSources, source);
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

const std::vector<audio::AudioSource*>& Scene::getAudioSources() const {
    return m_audioSources;
}

const std::vector<audio::AudioListener*>& Scene::getAudioListeners() const {
    return m_audioListeners;
}

const std::vector<scripting::ScriptSystem*> Scene::getScriptSystems() const {
    return m_systems;
}

const std::vector<std::string>& Scene::getTags() const {
    return m_tags;
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
