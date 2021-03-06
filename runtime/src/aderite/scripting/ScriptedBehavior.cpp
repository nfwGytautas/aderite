#include "ScriptedBehavior.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/io/SerializableAsset.hpp"
#include "aderite/scripting/BehaviorBase.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

ScriptedBehavior::ScriptedBehavior(BehaviorBase* behavior, scene::GameObject* gObject) : m_behaviorBase(behavior), m_gameObject(gObject) {
    m_instance = ::aderite::Engine::getScriptManager()->instantiate(m_behaviorBase->getClass());
    m_behaviorBase->m_instanceField.setValueType<ScriptedBehavior*>(m_instance, this);
}

void ScriptedBehavior::init() {
    if (m_behaviorBase == nullptr) {
        LOG_ERROR("[Scripting] Behavior base is null for scripted behavior");
        return;
    }

    if (m_behaviorBase->m_init) {
        m_behaviorBase->m_init(m_instance);
        m_initialized = true;
    }
}

void ScriptedBehavior::shutdown() {
    if (m_behaviorBase == nullptr) {
        LOG_ERROR("[Scripting] Behavior base is null for scripted behavior");
        return;
    }

    if (!m_initialized) {
        LOG_WARN("[Scripting] Non initialized scripted behavior is being shutdown, ignoring");
        return;
    }

    // TODO: Release asset references

    if (m_behaviorBase->m_shutdown) {
        m_behaviorBase->m_shutdown(m_instance);
        m_initialized = false;
    }
}

void ScriptedBehavior::update(float delta) {
    if (m_behaviorBase == nullptr) {
        LOG_ERROR("[Scripting] Behavior base is null for scripted behavior");
        return;
    }

    if (!m_initialized) {
        this->init();
    }

    if (m_behaviorBase->m_update) {
        m_behaviorBase->m_update(m_instance, delta);
    }
}

void ScriptedBehavior::onTriggerEnter(const physics::TriggerEvent& te) {
    if (m_behaviorBase->m_triggerEnter) {
        m_behaviorBase->m_triggerEnter(m_instance, ::aderite::Engine::getScriptManager()->getLocator().create(te));
    }
}

void ScriptedBehavior::onTriggerLeave(const physics::TriggerEvent& te) {
    if (m_behaviorBase->m_triggerLeave) {
        m_behaviorBase->m_triggerLeave(m_instance, ::aderite::Engine::getScriptManager()->getLocator().create(te));
    }
}

void ScriptedBehavior::onTriggerWasEntered(const physics::TriggerEvent& te) {
    if (m_behaviorBase->m_triggerWasEntered) {
        m_behaviorBase->m_triggerWasEntered(m_instance, ::aderite::Engine::getScriptManager()->getLocator().create(te));
    }
}

void ScriptedBehavior::onTriggerWasLeft(const physics::TriggerEvent& te) {
    if (m_behaviorBase->m_triggerWasLeft) {
        m_behaviorBase->m_triggerWasLeft(m_instance, ::aderite::Engine::getScriptManager()->getLocator().create(te));
    }
}

void ScriptedBehavior::onCollisionEnter(const physics::CollisionEvent& ce) {
    if (m_behaviorBase->m_collisionStart) {
        m_behaviorBase->m_collisionStart(m_instance, ::aderite::Engine::getScriptManager()->getLocator().create(ce));
    }
}

void ScriptedBehavior::onCollisionLeave(const physics::CollisionEvent& ce) {
    if (m_behaviorBase->m_collisionEnd) {
        m_behaviorBase->m_collisionEnd(m_instance, ::aderite::Engine::getScriptManager()->getLocator().create(ce));
    }
}

BehaviorBase* ScriptedBehavior::getBase() const {
    return m_behaviorBase;
}

MonoObject* ScriptedBehavior::getInstance() const {
    return m_instance;
}

scene::GameObject* ScriptedBehavior::getGameObject() const {
    return m_gameObject;
}

bool ScriptedBehavior::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "Script" << YAML::Value << m_behaviorBase->getName();
    emitter << YAML::Key << "Fields" << YAML::BeginMap;
    for (const FieldWrapper& field : m_behaviorBase->getFields()) {
        emitter << YAML::Key << field.getName() << YAML::Value;

        switch (field.getType()) {
        case FieldType::Float: {
            emitter << field.getValueType<float>(m_instance);
            break;
        }
        case FieldType::Boolean: {
            emitter << field.getValueType<bool>(m_instance);
            break;
        }
        case FieldType::Integer: {
            emitter << field.getValueType<int>(m_instance);
            break;
        }
        default: {
            io::SerializableObject* serializable = field.getSerializable(m_instance);

            if (serializable == nullptr) {
                emitter << YAML::Null;
            } else {
                switch (field.getType()) {
                case scripting::FieldType::Mesh:
                case scripting::FieldType::Material:
                case scripting::FieldType::Prefab:
                case scripting::FieldType::Audio: {
                    // SerializableObject
                    io::SerializableAsset* object = static_cast<io::SerializableAsset*>(serializable);
                    emitter << object->getHandle();
                    break;
                }
                default: {
                    ADERITE_ABORT("Unknown implementation for field type");
                }
                }
            }
        }
        }
    }
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;

    return true;
}

bool ScriptedBehavior::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (m_behaviorBase == nullptr) {
        return false;
    }

    for (const FieldWrapper& field : m_behaviorBase->getFields()) {
        const YAML::Node& fieldNode = data["Fields"][field.getName()];

        if (!fieldNode || fieldNode.IsNull()) {
            // Skip
            continue;
        }

        switch (field.getType()) {
        case scripting::FieldType::Float: {
            field.setValueType<float>(m_instance, fieldNode.as<float>());
            break;
        }
        case scripting::FieldType::Boolean: {
            field.setValueType<bool>(m_instance, fieldNode.as<bool>());
            break;
        }
        case scripting::FieldType::Integer: {
            field.setValueType<int>(m_instance, fieldNode.as<int>());
            break;
        }
        default: {
            switch (field.getType()) {
            case scripting::FieldType::Mesh:
            case scripting::FieldType::Material:
            case scripting::FieldType::Prefab:
            case scripting::FieldType::Audio: {
                // Serializable object
                io::SerializableAsset* asset = ::aderite::Engine::getAssetManager()->get(fieldNode.as<io::SerializableHandle>());
                asset->acquire();
                field.setSerializable(m_instance, asset);
                break;
            }
            default: {
                ADERITE_ABORT("Unknown implementation for field");
            }
            }
        }
        }
    }

    return true;
}

} // namespace scripting
} // namespace aderite
