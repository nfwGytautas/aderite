#include "Entity.hpp"

#include "aderite/io/Serializer.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scene {

Entity::Entity() : m_transform(new Transform()) {}

Entity::~Entity() {
    delete m_transform;
}

void Entity::addTag(size_t tag) {
    m_tags = m_tags | tag;
}

void Entity::removeTag(size_t tag) {
    m_tags = m_tags & ~tag;
}

bool Entity::hasTag(size_t tag) const {
    return m_tags & tag;
}

void Entity::setScene(Scene* scene) {
    m_scene = scene;

    // Add to new scene
    if (m_actor != nullptr) {
        m_scene->getPhysicsScene()->addActor(m_actor, m_transform);
    }
}

void Entity::setActor(physics::PhysicsActor* actor, bool keepColliders) {
    if (actor != nullptr) {
        if (keepColliders && m_actor != nullptr) {
            m_actor->transferColliders(actor);
        }

        // Add to scene
        if (m_scene != nullptr) {
            m_scene->getPhysicsScene()->addActor(actor, m_transform);
        }
    } else {
        if (keepColliders) {
            LOG_WARN("KeepColliders true on a nullptr actor");
        }
    }

    if (m_actor != nullptr) {
        if (m_scene != nullptr) {
            m_scene->getPhysicsScene()->detachActor(m_actor);
        }
        m_actor->setEntity(nullptr);
    }

    actor->setEntity(this);
    m_actor = actor;
}

void Entity::setRenderable(rendering::Renderable* renderable) {
    m_renderable = renderable;
}

reflection::Type Entity::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::ENTITY);
}

bool Entity::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Transform
    emitter << YAML::Key << "Transform";
    serializer->writeUntrackedType(emitter, m_transform);

    // Tag
    emitter << YAML::Key << "Tags" << YAML::Value << m_tags;

    // Actor
    emitter << YAML::Key << "Actor";
    if (m_actor != nullptr) {
        serializer->writeUntrackedType(emitter, m_actor);
    } else {
        emitter << YAML::Null;
    }

    // Renderable
    emitter << YAML::Key << "Renderable";
    if (m_renderable != nullptr) {
        serializer->writeUntrackedType(emitter, m_renderable);
    } else {
        emitter << YAML::Null;
    }

    return true;
}

bool Entity::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Transform
    serializer->fillData(m_transform, data["Transform"]);

    // Tags
    if (data["Tags"]) {
        m_tags = data["Tags"].as<size_t>();
    }

    // Actor
    if (data["Actor"] && !data["Actor"].IsNull()) {
        this->setActor(static_cast<physics::PhysicsActor*>(serializer->parseUntrackedType(data["Actor"])));
    }

    // Renderable
    if (data["Renderable"] && !data["Renderable"].IsNull()) {
        this->setRenderable(static_cast<rendering::Renderable*>(serializer->parseUntrackedType(data["Renderable"])));
    }

    return true;
}

} // namespace scene
} // namespace aderite
