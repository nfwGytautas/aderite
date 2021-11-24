#include "Entity.hpp"

#include "aderite/asset/PrefabAsset.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/Transform.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scene {

Entity::Entity() : m_transform(new Transform()) {}

Entity::~Entity() {
    delete m_transform;

    if (m_renderable != nullptr) {
        delete m_renderable;
    }
}

void Entity::addTag(size_t tag) {
    LOG_TRACE("[Scene] Added tag {0:b} to entity {1}", tag, this->getName());
    m_tags = m_tags | tag;

    // No longer valid
    m_prefab = nullptr;
}

void Entity::removeTag(size_t tag) {
    LOG_TRACE("[Scene] Removed tag {0:b} from entity {1}", tag, this->getName());
    m_tags = m_tags & ~tag;

    // No longer valid
    m_prefab = nullptr;
}

bool Entity::hasTag(size_t tag) const {
    return m_tags & tag;
}

void Entity::setScene(Scene* scene) {
    LOG_TRACE("[Scene] Setting {0} scene to {1:p}", this->getName(), static_cast<void*>(scene));

    if (m_scene != nullptr) {
        // Remove from previous scene
        m_scene->removeEntity(this);
    }

    m_scene = scene;

    // Add to new scene
    if (m_actor != nullptr && m_scene != nullptr) {
        m_scene->getPhysicsScene()->addActor(m_actor, m_transform);
    }
}

void Entity::setActor(physics::PhysicsActor* actor, bool keepColliders) {
    LOG_TRACE("[Scene] Setting {0} actor to {1:p}", static_cast<void*>(actor));
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
            LOG_WARN("[Scene] KeepColliders true on a nullptr actor");
        }
    }

    if (m_actor != nullptr) {
        if (m_scene != nullptr) {
            LOG_TRACE("[Scene] Detaching old actor {0:p} from {1}", static_cast<void*>(m_actor), this->getName());
            m_scene->getPhysicsScene()->detachActor(m_actor);
        }
        m_actor->setEntity(nullptr);
    }

    actor->setEntity(this);
    m_actor = actor;

    // No longer valid
    m_prefab = nullptr;
}

void Entity::setRenderable(rendering::Renderable* renderable) {
    m_renderable = renderable;
    m_prefab = nullptr;
}

void Entity::setName(const std::string& name) {
    LOG_TRACE("[Scene] Renaming entity {0} to {1}", m_name, name);
    m_name = name;
}

Scene* Entity::getScene() const {
    return m_scene;
}

Transform* Entity::getTransform() const {
    return m_transform;
}

physics::PhysicsActor* Entity::getActor() const {
    return m_actor;
}

rendering::Renderable* Entity::getRenderable() const {
    return m_renderable;
}

const size_t Entity::getTags() const {
    return m_tags;
}

const std::string& Entity::getName() const {
    return m_name;
}

Entity* Entity::clone() const {
    Entity* result = new Entity();

    // Copy asset type fields
    result->m_tags = m_tags;
    result->m_name = m_name;

    // Renderable
    if (m_renderable != nullptr) {
        result->setRenderable(m_renderable->clone());
    }

    // Physics actor
    if (m_actor != nullptr) {
        result->setActor(m_actor->clone());
    }

    // Add to scene
    if (m_scene != nullptr) {
        result->setScene(m_scene);
    }

    result->m_prefab = m_prefab;

    return result;
}

reflection::Type Entity::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::ENTITY);
}

bool Entity::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Prefab";

    if (m_prefab != nullptr) {
        // Entity was created using a prefab
        emitter << YAML::Value << m_prefab->getHandle();

        // Only the transform and name is serialized
        emitter << YAML::Key << "Transform";
        serializer->writeUntrackedType(emitter, m_transform);

        emitter << YAML::Key << "Name" << YAML::Value << m_name;
    } else {
        // Unique entity
        emitter << YAML::Null;

        // Transform
        emitter << YAML::Key << "Transform";
        serializer->writeUntrackedType(emitter, m_transform);

        // Tag
        emitter << YAML::Key << "Tags" << YAML::Value << m_tags;
        emitter << YAML::Key << "Name" << YAML::Value << m_name;

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
    }

    return true;
}

bool Entity::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (data["Prefab"] && !data["Prefab"].IsNull()) {
        // Entity created from prefab
        asset::PrefabAsset* prefab = static_cast<asset::PrefabAsset*>(serializer->getOrRead(data["Prefab"].as<io::SerializableHandle>()));
        Entity* prototype = prefab->getPrototype();

        // Copy asset type fields
        this->m_tags = prototype->getTags();

        // Renderable
        if (prototype->m_renderable != nullptr) {
            this->setRenderable(prototype->m_renderable->clone());
        }

        // Physics actor
        if (prototype->m_actor != nullptr) {
            this->setActor(prototype->m_actor->clone());
        }

        // Transform
        serializer->fillData(m_transform, data["Transform"]);

        if (data["Name"]) {
            m_name = data["Name"].as<std::string>();
        }

        this->m_prefab = prefab;
    } else {
        // Unique entity
        //
        // Transform
        serializer->fillData(m_transform, data["Transform"]);

        // Tags
        if (data["Tags"]) {
            m_tags = data["Tags"].as<size_t>();
        }

        if (data["Name"]) {
            m_name = data["Name"].as<std::string>();
        }

        // Actor
        if (data["Actor"] && !data["Actor"].IsNull()) {
            this->setActor(static_cast<physics::PhysicsActor*>(serializer->parseUntrackedType(data["Actor"])));
        }

        // Renderable
        if (data["Renderable"] && !data["Renderable"].IsNull()) {
            this->setRenderable(static_cast<rendering::Renderable*>(serializer->parseUntrackedType(data["Renderable"])));
        }
    }

    return true;
}

} // namespace scene
} // namespace aderite
