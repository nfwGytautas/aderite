#include "Scene.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/PrefabAsset.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"

namespace aderite {
namespace scene {

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
    m_gameObjects.clear();

    LOG_INFO("[Scene] Scene {0} deleted", this->getName());
}

void Scene::update(float delta) {
    // Free marked objects
    m_gameObjects.erase(std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
                                       [](const std::unique_ptr<GameObject>& gObject) {
                                           return gObject->isMarkedForDeletion();
                                       }),
                        m_gameObjects.end());

    // Update all game objects
    for (size_t i = 0; i < m_gameObjects.size(); i++) {
        m_gameObjects[i]->update(delta);
    }
}

GameObject* Scene::createGameObject() {
    static size_t nextId = 0;
    GameObject* go = new GameObject(this, "New object (" + std::to_string(nextId++) + ")");
    addObject(m_gameObjects, go);
    return go;
}

GameObject* Scene::createGameObject(asset::PrefabAsset* prefab) {
    GameObject* go = prefab->instantiate(this);
    addObject(m_gameObjects, go);
    return go;
}

void Scene::destroyGameObject(GameObject* object) {
    removeObject(m_gameObjects, object);
}

const std::vector<std::unique_ptr<GameObject>>& Scene::getGameObjects() const {
    return m_gameObjects;
}

reflection::Type Scene::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::SCENE);
}

bool Scene::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!PhysicsScene::serialize(serializer, emitter)) {
        return false;
    }

    // Objects
    emitter << YAML::Key << "GameObjects" << YAML::BeginSeq;
    for (const auto& object : m_gameObjects) {
        emitter << YAML::BeginMap;
        object->serialize(serializer, emitter);
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;
}

bool Scene::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!PhysicsScene::deserialize(serializer, data)) {
        return false;
    }

    // Objects
    auto objects = data["GameObjects"];
    if (objects) {
        for (auto object : objects) {
            scene::GameObject* gObject = this->createGameObject();
            gObject->deserialize(serializer, object);
        }
    }
}

Scene::Scene() {}

} // namespace scene
} // namespace aderite
