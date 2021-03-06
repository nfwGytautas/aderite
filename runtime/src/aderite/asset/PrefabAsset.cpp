#include "PrefabAsset.hpp"

#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioListenerData.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/audio/AudioSourceData.hpp"
#include "aderite/physics/PhysXActor.hpp"
#include "aderite/physics/PhysicsProperties.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/scene/CameraSettings.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scripting/BehaviorBase.hpp"
#include "aderite/scripting/ScriptedBehavior.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace asset {

PrefabAsset::PrefabAsset() {}

PrefabAsset::PrefabAsset(scene::GameObject* gObject) {
    this->setName(gObject->getName());

    scene::TransformProvider* const transform = gObject->getTransform();
    rendering::Renderable* const renderable = gObject->getRenderable();
    physics::PhysXActor* const actor = gObject->getActor();
    scene::Camera* const camera = gObject->getCamera();
    audio::AudioListener* const listener = gObject->getAudioListener();
    audio::AudioSource* const source = gObject->getAudioSource();
    std::vector<scripting::ScriptedBehavior*> behaviors = gObject->getBehaviors();

    if (transform != nullptr) {
        m_transform = new scene::TransformProvider();
        *m_transform = *transform;
    }

    if (renderable != nullptr) {
        m_renderable = new rendering::RenderableData();
        *m_renderable = renderable->getData();
    }

    if (actor != nullptr) {
        m_actor = new physics::PhysicsProperties();
        *m_actor = actor->getData();
    }

    if (camera != nullptr) {
        m_camera = new scene::CameraSettings();
        *m_camera = camera->getData();
    }

    if (listener != nullptr) {
        m_audioListener = new audio::AudioListenerData();
        *m_audioListener = listener->getData();
    }

    if (source != nullptr) {
        m_audioSource = new audio::AudioSourceData();
        *m_audioSource = source->getData();
    }

    if (!behaviors.empty()) {
        for (scripting::ScriptedBehavior* behavior : behaviors) {
            // Nullptr game object, because this won't actually be used as a behavior only as a data storage
            scripting::ScriptedBehavior* copy = new scripting::ScriptedBehavior(behavior->getBase(), nullptr);
            copy->getBase()->copyOver(behavior, copy);
            m_behaviors.push_back(copy);
        }
    }
}

PrefabAsset::~PrefabAsset() {
    LOG_TRACE("[Asset] Destroying {0}", this->getHandle());

    delete m_transform;
    delete m_renderable;
    delete m_actor;
    delete m_camera;
    delete m_audioListener;
    delete m_audioSource;

    for (scripting::ScriptedBehavior* behavior : m_behaviors) {
        delete behavior;
    }
}

scene::GameObject* PrefabAsset::instantiate(scene::Scene* scene) const {
    static size_t nextId = 0;
    scene::GameObject* go = scene->createGameObject();
    go->setName(this->getName() + " " + std::to_string(nextId++));

    if (m_transform != nullptr) {
        *go->addTransform() = *m_transform;
    }

    if (m_renderable != nullptr) {
        go->addRenderable()->getData() = *m_renderable;
    }

    if (m_actor != nullptr) {
        go->addActor()->getData() = *m_actor;
    }

    if (m_camera != nullptr) {
        go->addCamera()->getData() = *m_camera;
    }

    if (m_audioListener != nullptr) {
        go->addAudioListener()->getData() = *m_audioListener;
    }

    if (m_audioSource != nullptr) {
        go->addAudioSource()->getData() = *m_audioSource;
    }

    for (scripting::ScriptedBehavior* behavior : m_behaviors) {
        scripting::ScriptedBehavior* copy = new scripting::ScriptedBehavior(behavior->getBase(), go);
        behavior->getBase()->copyOver(behavior, copy);
        go->addBehavior(copy);
    }

    return go;
}

void PrefabAsset::load(const io::Loader* loader) {
    LOG_TRACE("[Asset] Loading {0}", this->getName());
}

void PrefabAsset::unload() {
    LOG_TRACE("[Asset] Unloading {0}", this->getName());
}

bool PrefabAsset::needsLoading() const {
    return false;
}

reflection::Type PrefabAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::PREFAB);
}

bool PrefabAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (m_transform != nullptr) {
        if (!m_transform->serialize(serializer, emitter)) {
            return false;
        }
    }

    if (m_renderable != nullptr) {
        if (!m_renderable->serialize(serializer, emitter)) {
            return false;
        }
    }

    if (m_actor != nullptr) {
        if (!m_actor->serialize(serializer, emitter)) {
            return false;
        }
    }

    if (m_camera != nullptr) {
        if (!m_camera->serialize(serializer, emitter)) {
            return false;
        }
    }

    if (m_audioListener != nullptr) {
        if (!m_audioListener->serialize(serializer, emitter)) {
            return false;
        }
    }

    if (m_audioSource != nullptr) {
        if (!m_audioSource->serialize(serializer, emitter)) {
            return false;
        }
    }

    emitter << YAML::Key << "Behaviors" << YAML::BeginSeq;
    for (scripting::ScriptedBehavior* behavior : m_behaviors) {
        if (!behavior->serialize(serializer, emitter)) {
            return false;
        }
    }
    emitter << YAML::EndSeq;

    return true;
}

bool PrefabAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Create component instances
    {
        const YAML::Node& componentNode = data["Transform"];
        if (componentNode && !componentNode.IsNull()) {
            m_transform = new scene::TransformProvider();
            if (!m_transform->deserialize(serializer, data)) {
                return false;
            }
        }
    }

    {
        const YAML::Node& componentNode = data["Renderable"];
        if (componentNode && !componentNode.IsNull()) {
            m_renderable = new rendering::RenderableData();
            if (!m_renderable->deserialize(serializer, data)) {
                return false;
            }
        }
    }

    {
        const YAML::Node& componentNode = data["PhysicsActor"];
        if (componentNode && !componentNode.IsNull()) {
            m_actor = new physics::PhysicsProperties();
            if (!m_actor->deserialize(serializer, data)) {
                return false;
            }
        }
    }

    {
        const YAML::Node& componentNode = data["Camera"];
        if (componentNode && !componentNode.IsNull()) {
            m_camera = new scene::CameraSettings();
            if (!m_camera->deserialize(serializer, data)) {
                return false;
            }
        }
    }

    {
        const YAML::Node& componentNode = data["AudioListener"];
        if (componentNode && !componentNode.IsNull()) {
            m_audioListener = new audio::AudioListenerData();
            if (!m_audioListener->deserialize(serializer, data)) {
                return false;
            }
        }
    }

    {
        const YAML::Node& componentNode = data["AudioSource"];
        if (componentNode && !componentNode.IsNull()) {
            m_audioSource = new audio::AudioSourceData();
            if (!m_audioSource->deserialize(serializer, data)) {
                return false;
            }
        }
    }

    {
        for (const YAML::Node& scriptNode : data["Behaviors"]) {
            scripting::BehaviorBase* behaviorBase =
                ::aderite::Engine::getScriptManager()->getBehavior(scriptNode["Script"].as<std::string>());
            if (behaviorBase == nullptr) {
                LOG_WARN("[Scripting] Behavior with name {0} no longer found so is skipped", scriptNode["Script"].as<std::string>());
                continue;
            }

            scripting::ScriptedBehavior* behavior = new scripting::ScriptedBehavior(behaviorBase, nullptr);
            if (!behavior->deserialize(serializer, scriptNode)) {
                delete behavior;
                return false;
            }

            m_behaviors.push_back(behavior);
        }
    }

    return true;
}

} // namespace asset
} // namespace aderite
