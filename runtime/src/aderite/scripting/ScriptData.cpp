#include "ScriptData.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scripting/ScriptClass.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

ScriptData::ScriptData(scene::Scene* scene) : m_scene(scene) {}

const char* ScriptData::getScriptName() const {
    if (m_instance == nullptr) {
        return "";
    }

    return mono_class_get_name(mono_object_get_class(m_instance));
}

ScriptClass* ScriptData::getClass() const {
    return m_klass;
}

void ScriptData::setScriptName(const std::string& name) {
    ADERITE_DYNAMIC_ASSERT(this->isEmpty(), "Tried to set a non empty script data object");

    m_klass = ::aderite::Engine::getScriptManager()->getScript(name);
    m_instance = m_klass->createInstance();
}

bool ScriptData::isEmpty() const {
    return m_instance == nullptr;
}

void ScriptData::clear() {
    // TODO: Delete instance
    m_instance = nullptr;
}

void ScriptData::load() {
    m_klass->loadData(this);
}

MonoObject* ScriptData::getInstance() const {
    return m_instance;
}

bool ScriptData::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (this->isEmpty()) {
        // Bad class
        return false;
    }

    emitter << YAML::BeginMap;
    emitter << YAML::Key << "Script" << YAML::Value << this->getScriptName();
    emitter << YAML::Key << "Fields" << YAML::BeginMap;
    for (const FieldWrapper& field : this->m_klass->getFields()) {
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
                case scripting::FieldType::AudioSource: {
                    audio::AudioSource* source = static_cast<audio::AudioSource*>(serializable);
                    emitter << source->getName();
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

bool ScriptData::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    this->clear();

    this->setScriptName(data["Script"].as<std::string>());

    if (this->isEmpty()) {
        // Bad class
        return false;
    }

    for (const FieldWrapper& field : this->m_klass->getFields()) {
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
            case scripting::FieldType::AudioSource: {
                audio::AudioSource* source = m_scene->getSource(fieldNode.as<std::string>());
                field.setSerializable(m_instance, source);
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
