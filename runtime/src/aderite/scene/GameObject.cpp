#include "GameObject.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/PhysXActor.hpp"
#include "aderite/physics/PhysicsEventList.hpp"
#include "aderite/physics/geometry/Geometry.hpp"
#include "aderite/rendering/FrameData.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/ScriptedBehavior.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scene {

GameObject::GameObject(scene::Scene* scene, const std::string& name) : m_scene(scene) {
    this->setName(name);
}

GameObject::~GameObject() {
    if (m_transform != nullptr) {
        delete m_transform;
    }

    if (m_renderable != nullptr) {
        delete m_renderable;
    }

    if (m_actor != nullptr) {
        delete m_actor;
    }

    if (m_camera != nullptr) {
        delete m_camera;
    }

    if (m_audioSource != nullptr) {
        delete m_audioSource;
    }

    if (m_audioListener != nullptr) {
        delete m_audioListener;
    }
}

void GameObject::update(float delta) {
    rendering::FrameData& fd = ::aderite::Engine::getRenderer()->getWriteFrameData();

    if (m_renderable != nullptr) {
        m_renderable->update(delta);
    }

    // TODO: Don't update some components, when not in play mode

    if (m_camera != nullptr) {
        m_camera->update(delta);
    }

    if (m_actor != nullptr) {
        m_actor->update(delta);
    }

    if (m_audioSource != nullptr) {
        m_audioSource->update(delta);
    }

    if (m_audioListener != nullptr) {
        m_audioListener->update(delta);
    }

    if (m_transform != nullptr) {
        m_transform->resetModifiedFlag();
    }
}

void GameObject::onTriggerEnter(const physics::TriggerEvent& te) {
    LOG_TRACE("{0} entered trigger {1}", this->getName(), te.Trigger->getActor()->getGameObject()->getName());
}

void GameObject::onTriggerLeave(const physics::TriggerEvent& te) {
    LOG_TRACE("{0} left trigger {1}", this->getName(), te.Trigger->getActor()->getGameObject()->getName());
}

void GameObject::onTriggerWasEntered(const physics::TriggerEvent& te) {
    LOG_TRACE("Trigger {0} was entered by {1}", this->getName(), te.Actor->getActor()->getGameObject()->getName());
}

void GameObject::onTriggerWasLeft(const physics::TriggerEvent& te) {
    LOG_TRACE("Trigger {0} was left by {1}", this->getName(), te.Actor->getActor()->getGameObject()->getName());
}

void GameObject::onCollisionEnter(const physics::CollisionEvent& ce) {
    LOG_TRACE("{0} collision start with {1}", this->getName(), ce.Actor2->getActor()->getGameObject()->getName());
}

void GameObject::onCollisionLeave(const physics::CollisionEvent& ce) {
    LOG_TRACE("{0} collision end with {1}", this->getName(), ce.Actor2->getActor()->getGameObject()->getName());
}

void GameObject::markForDeletion() {
    m_markedForDeletion = true;
}

bool GameObject::isMarkedForDeletion() const {
    return m_markedForDeletion;
}

Scene* GameObject::getScene() const {
    return m_scene;
}

TransformProvider* GameObject::addTransform() {
    ADERITE_DYNAMIC_ASSERT(m_transform == nullptr, "Tried to add a transform to an object that already has one");
    m_transform = new TransformProvider();
    return m_transform;
}

void GameObject::removeTransform() {
    ADERITE_DYNAMIC_ASSERT(m_transform != nullptr, "Tried to remove transform from object that doesn't have one");
    delete m_transform;
    m_transform = nullptr;
}

TransformProvider* GameObject::getTransform() const {
    return m_transform;
}

rendering::Renderable* GameObject::addRenderable() {
    ADERITE_DYNAMIC_ASSERT(m_renderable == nullptr, "Tried to add a renderable to an object that already has one");
    if (m_transform == nullptr) {
        this->addTransform();
    }
    m_renderable = new rendering::Renderable(this);
    return m_renderable;
}

void GameObject::removeRenderable() {
    ADERITE_DYNAMIC_ASSERT(m_renderable != nullptr, "Tried to remove renderable from object that doesn't have one");
    delete m_renderable;
    m_renderable = nullptr;
}

rendering::Renderable* GameObject::getRenderable() const {
    return m_renderable;
}

physics::PhysXActor* GameObject::addActor() {
    ADERITE_DYNAMIC_ASSERT(m_actor == nullptr, "Tried to add a actor to an object that already has one");
    if (m_transform == nullptr) {
        this->addTransform();
    }
    m_actor = new physics::PhysXActor(this);
    return m_actor;
}

void GameObject::removeActor() {
    ADERITE_DYNAMIC_ASSERT(m_actor != nullptr, "Tried to remove actor from object that doesn't have one");
    delete m_actor;
    m_actor = nullptr;
}

physics::PhysXActor* GameObject::getActor() const {
    return m_actor;
}

Camera* GameObject::addCamera() {
    ADERITE_DYNAMIC_ASSERT(m_camera == nullptr, "Tried to add a camera to an object that already has one");
    if (m_transform == nullptr) {
        this->addTransform();
    }

    m_camera = new Camera(this);
    return m_camera;
}

void GameObject::removeCamera() {
    ADERITE_DYNAMIC_ASSERT(m_camera != nullptr, "Tried to remove camera from object that doesn't have one");
    delete m_camera;
    m_camera = nullptr;
}

Camera* GameObject::getCamera() const {
    return m_camera;
}

audio::AudioSource* GameObject::addAudioSource() {
    ADERITE_DYNAMIC_ASSERT(m_audioSource == nullptr, "Tried to add a audio source to an object that already has one");
    if (m_transform == nullptr) {
        this->addTransform();
    }

    m_audioSource = new audio::AudioSource(this);
    return m_audioSource;
}

void GameObject::removeAudioSource() {
    ADERITE_DYNAMIC_ASSERT(m_audioSource != nullptr, "Tried to remove audio source from object that doesn't have one");
    delete m_audioSource;
    m_audioSource = nullptr;
}

audio::AudioSource* GameObject::getAudioSource() const {
    return m_audioSource;
}

audio::AudioListener* GameObject::addAudioListener() {
    ADERITE_DYNAMIC_ASSERT(m_audioListener == nullptr, "Tried to add a audio listener to an object that already has one");
    if (m_transform == nullptr) {
        this->addTransform();
    }

    m_audioListener = new audio::AudioListener(this);
    return m_audioListener;
}

void GameObject::removeAudioListener() {
    ADERITE_DYNAMIC_ASSERT(m_audioListener != nullptr, "Tried to remove audio listener from object that doesn't have one");
    delete m_audioListener;
    m_audioListener = nullptr;
}

audio::AudioListener* GameObject::getAudioListener() const {
    return m_audioListener;
}

void GameObject::addBehavior(scripting::ScriptedBehavior* behavior) {
    m_behaviors.push_back(behavior);
}

void GameObject::removeBehavior(scripting::ScriptedBehavior* behavior) {
    m_behaviors.erase(std::find(m_behaviors.begin(), m_behaviors.end(), behavior));
}

std::vector<scripting::ScriptedBehavior*> GameObject::getBehaviors() const {
    return m_behaviors;
}

bool GameObject::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "GameObject" << YAML::BeginMap;
    emitter << YAML::Key << "Name" << YAML::Value << this->getName();

    if (this->getTransform() != nullptr) {
        if (!this->getTransform()->serialize(serializer, emitter)) {
            return false;
        }
    }

    if (m_renderable != nullptr) {
        if (!m_renderable->getData().serialize(serializer, emitter)) {
            return false;
        }
    }

    if (m_actor != nullptr) {
        if (!m_actor->getData().serialize(serializer, emitter)) {
            return false;
        }
    }

    if (m_camera != nullptr) {
        if (!m_camera->getData().serialize(serializer, emitter)) {
            return false;
        }
    }

    if (m_audioSource != nullptr) {
        if (!m_audioSource->getData().serialize(serializer, emitter)) {
            return false;
        }
    }

    if (m_audioListener != nullptr) {
        if (!m_audioListener->getData().serialize(serializer, emitter)) {
            return false;
        }
    }

    emitter << YAML::Key << "Behaviors" << YAML::BeginSeq;
    for (scripting::ScriptedBehavior* sb : m_behaviors) {
        if (!sb->serialize(serializer, emitter)) {
            return false;
        }
    }
    emitter << YAML::EndSeq;

    emitter << YAML::EndMap;
    return true;
}

bool GameObject::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& gameObject = data["GameObject"];
    if (!gameObject || gameObject.IsNull()) {
        return false;
    }

    this->setName(gameObject["Name"].as<std::string>());

    // Create component instances
    {
        const YAML::Node& componentNode = gameObject["Transform"];
        if (componentNode && !componentNode.IsNull()) {
            if (!this->addTransform()->deserialize(serializer, gameObject)) {
                return false;
            }
        }
    }

    {
        const YAML::Node& componentNode = gameObject["Renderable"];
        if (componentNode && !componentNode.IsNull()) {
            if (!this->addRenderable()->getData().deserialize(serializer, gameObject)) {
                return false;
            }
        }
    }

    {
        const YAML::Node& componentNode = gameObject["PhysicsActor"];
        if (componentNode && !componentNode.IsNull()) {
            if (!this->addActor()->getData().deserialize(serializer, gameObject)) {
                return false;
            }
        }
    }

    {
        const YAML::Node& componentNode = gameObject["Camera"];
        if (componentNode && !componentNode.IsNull()) {
            if (!this->addCamera()->getData().deserialize(serializer, gameObject)) {
                return false;
            }
        }
    }

    {
        const YAML::Node& componentNode = gameObject["AudioSource"];
        if (componentNode && !componentNode.IsNull()) {
            if (!this->addAudioSource()->getData().deserialize(serializer, gameObject)) {
                return false;
            }
        }
    }

    {
        const YAML::Node& componentNode = gameObject["AudioListener"];
        if (componentNode && !componentNode.IsNull()) {
            if (!this->addAudioListener()->getData().deserialize(serializer, gameObject)) {
                return false;
            }
        }
    }

    {
        for (const YAML::Node& scriptNode : gameObject["Behaviors"]) {
            scripting::BehaviorBase* behaviorBase = ::aderite::Engine::getScriptManager()->getBehavior(scriptNode["Script"].as<std::string>());
            if (behaviorBase == nullptr) {
                LOG_WARN("[Scripting] Behavior with name {0} no longer found so is skipped", scriptNode["Script"].as<std::string>());
                continue;
            }

            scripting::ScriptedBehavior* behavior = new scripting::ScriptedBehavior(behaviorBase);
            if (!behavior->deserialize(serializer, scriptNode)) {
                delete behavior;
                return false;
            }

            this->addBehavior(behavior);
        }
    }

    return true;
}

reflection::Type GameObject::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::GAME_OBJECT);
}

} // namespace scene
} // namespace aderite
