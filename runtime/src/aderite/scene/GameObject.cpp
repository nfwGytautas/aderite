#include "GameObject.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/physics/PhysXActor.hpp"
#include "aderite/rendering/FrameData.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/scene/Camera.hpp"
#include "aderite/scene/StandaloneTransformProvider.hpp"

namespace aderite {
namespace scene {

inline glm::mat4 calculateTransformationMatrix(scene::TransformProvider* transform) {
    glm::vec3 position = transform->getPosition();
    glm::quat rotation = transform->getRotation();
    glm::vec3 scale = transform->getScale();

    glm::mat4 rMat = glm::toMat4(rotation);
    return glm::translate(glm::mat4(1.0f), position) * rMat * glm::scale(glm::mat4(1.0f), scale);
}

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
}

void GameObject::update(float delta) {
    rendering::FrameData& fd = ::aderite::Engine::getRenderer()->getWriteFrameData();

    if (m_transform != nullptr) {
        // Renderable
        if (m_renderable != nullptr && m_renderable->isValid()) {
            rendering::DrawCall& dc = fd.DrawCalls[m_renderable->hash()];
            dc.Renderable = m_renderable;
            dc.Transformations.push_back(calculateTransformationMatrix(m_transform));
        }

        // Camera
        if (m_camera != nullptr) {
            // Fill camera data
            rendering::CameraData cd;
            cd.Name = this->getName();
            cd.Output = m_camera->getOutputHandle();
            cd.ProjectionMatrix = glm::perspective(glm::radians(m_camera->getFoV()), 1.0f, 0.1f, 1000.0f);
            cd.ViewMatrix =
                glm::inverse(glm::translate(glm::mat4(1.0f), m_transform->getPosition()) * glm::toMat4(m_transform->getRotation()));

            // Push to the list
            fd.Cameras.push_back(cd);
        }
    }
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

void GameObject::addTransform() {
    ADERITE_DYNAMIC_ASSERT(m_transform == nullptr, "Tried to add a transform to an object that already has one");
    m_transform = new StandaloneTransformProvider();
}

void GameObject::removeTransform() {
    ADERITE_DYNAMIC_ASSERT(m_transform != nullptr, "Tried to remove transform from object that doesn't have one");
    delete m_transform;
    m_transform = nullptr;
}

TransformProvider* GameObject::getTransform() const {
    if (m_actor != nullptr) {
        return m_actor;
    }

    return m_transform;
}

void GameObject::addRenderable() {
    ADERITE_DYNAMIC_ASSERT(m_renderable == nullptr, "Tried to add a renderable to an object that already has one");
    if (m_transform == nullptr) {
        this->addTransform();
    }
    m_renderable = new rendering::Renderable();
}

void GameObject::removeRenderable() {
    ADERITE_DYNAMIC_ASSERT(m_renderable != nullptr, "Tried to remove renderable from object that doesn't have one");
    delete m_renderable;
    m_renderable = nullptr;
}

rendering::Renderable* GameObject::getRenderable() const {
    return m_renderable;
}

void GameObject::addActor() {
    ADERITE_DYNAMIC_ASSERT(m_actor == nullptr, "Tried to add a actor to an object that already has one");
    m_actor = new physics::PhysXActor(this);

    // Static by default
    m_actor->makeStatic();

    if (m_transform != nullptr) {
        m_actor->setPosition(m_transform->getPosition());
        m_actor->setRotation(m_transform->getRotation());
        m_actor->setScale(m_transform->getScale());
    }
}

void GameObject::removeActor() {
    ADERITE_DYNAMIC_ASSERT(m_actor != nullptr, "Tried to remove actor from object that doesn't have one");

    if (m_transform != nullptr) {
        m_transform->setPosition(m_actor->getPosition());
        m_transform->setRotation(m_actor->getRotation());
        m_transform->setScale(m_actor->getScale());
    }

    delete m_actor;
    m_actor = nullptr;
}

physics::PhysXActor* GameObject::getActor() {
    return m_actor;
}

void GameObject::addCamera() {
    ADERITE_DYNAMIC_ASSERT(m_camera == nullptr, "Tried to add a camera to an object that already has one");
    if (m_transform == nullptr) {
        this->addTransform();
    }

    m_camera = new Camera();
}

void GameObject::removeCamera() {
    ADERITE_DYNAMIC_ASSERT(m_camera != nullptr, "Tried to remove camera from object that doesn't have one");
    delete m_camera;
    m_camera = nullptr;
}

Camera* GameObject::getCamera() {
    return m_camera;
}

bool GameObject::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    return true;
}

bool GameObject::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    return true;
}

reflection::Type GameObject::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::GAME_OBJECT);
}

} // namespace scene
} // namespace aderite
