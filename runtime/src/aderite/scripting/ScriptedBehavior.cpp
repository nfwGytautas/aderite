#include "ScriptedBehavior.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/io/SerializableAsset.hpp"
#include "aderite/scripting/BehaviorBase.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace aderite {
namespace scripting {

ScriptedBehavior::ScriptedBehavior(BehaviorBase* behavior) : m_behaviorBase(behavior) {
    m_instance = ::aderite::Engine::getScriptManager()->instantiate(m_behaviorBase->getClass());
}

BehaviorBase* ScriptedBehavior::getBase() const {
    return m_behaviorBase;
}

MonoObject* ScriptedBehavior::getInstance() const {
    return m_instance;
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
            case scripting::FieldType::Audio: {
                // Serializable object
                io::SerializableObject* object = ::aderite::Engine::getAssetManager()->get(fieldNode.as<io::SerializableHandle>());
                field.setSerializable(m_instance, object);
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
