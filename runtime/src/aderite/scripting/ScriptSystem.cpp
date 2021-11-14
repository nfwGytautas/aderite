#include "ScriptSystem.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/scene/EntitySelector.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"

#include "aderite/audio/AudioSource.hpp "

namespace aderite {
namespace scripting {

ScriptSystem::ScriptSystem() {}

void ScriptSystem::setSelector(scene::EntitySelector* selector) {
    m_selector = selector;
}

void ScriptSystem::update(float delta) {
    if (m_update) {
        this->updateEntitiesArray();
        m_update(delta, m_entities);
    }
}

void ScriptSystem::init() {
    LOG_TRACE("[Scripting] Initialzing {0}", m_name);
    if (m_init) {
        m_init();
    }
}

void ScriptSystem::shutdown() {
    LOG_TRACE("[Scripting] Shutting down {0}", m_name);
    if (m_shutdown) {
        m_shutdown();
    }
}

void ScriptSystem::triggerEnter(MonoObject* triggerEvent) {
    if (m_triggerEnter) {
        m_triggerEnter(triggerEvent);
    }
}

void ScriptSystem::triggerLeave(MonoObject* triggerEvent) {
    if (m_triggerLeave) {
        m_triggerLeave(triggerEvent);
    }
}

void ScriptSystem::collisionStart(MonoObject* collisionEvent) {
    if (m_collisionStart) {
        m_collisionStart(collisionEvent);
    }
}

void ScriptSystem::collisionEnd(MonoObject* collisionEvent) {
    if (m_collisionEnd) {
        m_collisionEnd(collisionEvent);
    }
}

void ScriptSystem::load(const std::string& name) {
    LOG_TRACE("[Scripting] Loading {0} into {1:p}", name, static_cast<void*>(this));
    ScriptManager* sm = ::aderite::Engine::getScriptManager();

    // Resolve class
    m_klass = sm->getSystemClass(name);
    if (m_klass == nullptr) {
        LOG_ERROR("[Scripting] Failed to resolve mono class for {0}", name);
        return;
    }

    // Create instance
    LOG_TRACE("[Scripting] Create instance");
    m_instance = sm->instantiate(m_klass);

    // Resolve methods and fields
    LOG_TRACE("[Scripting] Resolving methods");
    m_init = ThunkedMethod<void>(sm->getMethod(m_klass, "Init", 0), m_instance);
    m_shutdown = ThunkedMethod<void>(sm->getMethod(m_klass, "Shutdown", 0), m_instance);
    m_update = ThunkedMethod<void, float, MonoArray*>(sm->getMethod(m_klass, "Update", 2), m_instance);

    m_triggerEnter = ThunkedMethod<void, MonoObject*>(sm->getMethod(m_klass, "OnTriggerEnter", 1), m_instance);
    m_triggerLeave = ThunkedMethod<void, MonoObject*>(sm->getMethod(m_klass, "OnTriggerLeave", 1), m_instance);
    m_collisionStart = ThunkedMethod<void, MonoObject*>(sm->getMethod(m_klass, "OnCollisionStart", 1), m_instance);
    m_collisionEnd = ThunkedMethod<void, MonoObject*>(sm->getMethod(m_klass, "OnCollisionEnd", 1), m_instance);

    // User defined fields
    LOG_TRACE("[Scripting] Resolving public fields");
    m_fields = sm->getPublicFields(m_instance);

    LOG_INFO("[Scripting] System {0:p} loaded", static_cast<void*>(this));

    m_name = name;
}

void ScriptSystem::attachToScene(scene::Scene* scene) {
    LOG_TRACE("[Scripting] Attaching {0} to {1}", m_name, scene->getHandle());
    m_scene = scene;
}

bool ScriptSystem::hasField(const std::string& name) const {
    return std::find_if(m_fields.begin(), m_fields.end(), [name](const FieldWrapper& wrapper) {
               return wrapper.getName() == name;
           }) != m_fields.end();
}

const FieldWrapper& ScriptSystem::getField(const std::string& name) const {
    return *std::find_if(m_fields.begin(), m_fields.end(), [name](const FieldWrapper& wrapper) {
        return wrapper.getName() == name;
    });
}

reflection::Type ScriptSystem::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::SCRIPT_SYSTEM);
}

bool ScriptSystem::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (m_klass == nullptr) {
        return false;
    }

    // Meta information
    emitter << YAML::Key << "Name" << YAML::Value << m_name;

    // Selector
    emitter << YAML::Key << "Selector" << YAML::Value;
    if (m_selector != nullptr) {
        emitter << m_selector->getName();
    } else {
        emitter << YAML::Null;
    }

    // Fields
    emitter << YAML::Key << "Fields" << YAML::BeginMap;
    for (const scripting::FieldWrapper& fw : m_fields) {
        emitter << YAML::Key << fw.getName() << YAML::Value;

        switch (fw.getType()) {
        case scripting::FieldType::Float: {
            emitter << (*(float*)scripting::unbox(fw.getValueObject()));
            break;
        }
        case scripting::FieldType::Boolean: {
            emitter << (*(bool*)scripting::unbox(fw.getValueObject()));
            break;
        }
        case scripting::FieldType::Mesh: {
            MonoObject* mesh = nullptr;
            fw.getValue(&mesh);
            if (mesh == nullptr) {
                emitter << YAML::Null;
            } else {
                asset::MeshAsset* meshAsset = nullptr;
                scripting::extract(mesh, meshAsset);
                emitter << meshAsset->getHandle();
            }
            break;
        }
        case scripting::FieldType::Material: {
            MonoObject* material = nullptr;
            fw.getValue(&material);
            if (material == nullptr) {
                emitter << YAML::Null;
            } else {
                asset::MaterialAsset* materialAsset = nullptr;
                scripting::extract(material, materialAsset);
                emitter << materialAsset->getHandle();
            }
            break;
        }
        case scripting::FieldType::Audio: {
            MonoObject* audio = nullptr;
            fw.getValue(&audio);
            if (audio == nullptr) {
                emitter << YAML::Null;
            } else {
                asset::AudioAsset* audioAsset = nullptr;
                scripting::extract(audio, audioAsset);
                emitter << audioAsset->getHandle();
            }
            break;
        }
        case scripting::FieldType::AudioSource: {
            MonoObject* audioSource = nullptr;
            fw.getValue(&audioSource);
            if (audioSource == nullptr) {
                emitter << YAML::Null;
            } else {
                audio::AudioSource* source = nullptr;
                scripting::extract(audioSource, source);
                emitter << source->getName();
            }
            break;
        }
        default: {
            emitter << YAML::Null;
        }
        }
    }
    emitter << YAML::EndMap;

    return true;
}

bool ScriptSystem::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!data["Name"]) {
        LOG_ERROR("[Scripting] Can't deserialize ScriptSystem without Name node");
        return false;
    }

    // Meta
    const std::string& name = data["Name"].as<std::string>();

    // Load instance
    this->load(name);

    if (m_instance == nullptr) {
        // No instance abort
        return false;
    }

    // Selector
    if (data["Selector"] && !data["Selector"].IsNull()) {
        m_selector = m_scene->getSelector(data["Selector"].as<std::string>());
    }

    // Fields
    for (auto& fieldData : data["Fields"]) {
        const std::string fieldName = fieldData.first.as<std::string>();

        if (!this->hasField(fieldName)) {
            LOG_WARN("[Scripting] Unexpected field {0}, skipping", fieldName);
            continue;
        }

        const scripting::FieldWrapper& fw = this->getField(fieldName);

        if (!fieldData.second.IsNull()) {
            switch (fw.getType()) {
            case scripting::FieldType::Float: {
                float val = fieldData.second.as<float>();
                fw.setValue(&val);
                break;
            }
            case scripting::FieldType::Boolean: {
                bool val = fieldData.second.as<bool>();
                fw.setValue(&val);
                break;
            }
            case scripting::FieldType::Mesh: {
                asset::MeshAsset* mesh = static_cast<asset::MeshAsset*>(
                    ::aderite::Engine::getSerializer()->getOrRead(fieldData.second.as<io::SerializableHandle>()));
                MonoObject* obj = ::aderite::Engine::getScriptManager()->getLocator().create(mesh);
                fw.setValue(obj);
                break;
            }
            case scripting::FieldType::Material: {
                asset::MaterialAsset* material = static_cast<asset::MaterialAsset*>(
                    ::aderite::Engine::getSerializer()->getOrRead(fieldData.second.as<io::SerializableHandle>()));
                MonoObject* obj = ::aderite::Engine::getScriptManager()->getLocator().create(material);
                fw.setValue(obj);
                break;
            }
            case scripting::FieldType::Audio: {
                asset::AudioAsset* audio = static_cast<asset::AudioAsset*>(
                    ::aderite::Engine::getSerializer()->getOrRead(fieldData.second.as<io::SerializableHandle>()));
                MonoObject* obj = ::aderite::Engine::getScriptManager()->getLocator().create(audio);
                fw.setValue(obj);
                break;
            }
            case scripting::FieldType::AudioSource: {
                audio::AudioSource* source = m_scene->getSource(fieldData.second.as<std::string>());
                MonoObject* obj = ::aderite::Engine::getScriptManager()->getLocator().create(source);
                fw.setValue(obj);
                break;
            }
            }
        }
    }

    return true;
}

void ScriptSystem::updateEntitiesArray() {
    size_t length = m_selector != nullptr ? m_selector->size() : 0;

    auto& locator = ::aderite::Engine::getScriptManager()->getLocator();

    if (m_entities == nullptr) {
        // Create
        m_entities = mono_array_new(::aderite::Engine::getScriptManager()->getDomain(), locator.getEntity().Klass, length);
    } else {
        // Check if there is a need for realloc
        if (length != mono_array_length(m_entities)) {
            LOG_TRACE("[Scripting] Reallocating entity array for system {0}, previous length: {1}, new length: {2}", m_name,
                      mono_array_length(m_entities), length);
            m_entities = mono_array_new(::aderite::Engine::getScriptManager()->getDomain(), locator.getEntity().Klass, length);
        }
    }

    if (m_selector == nullptr) {
        return;
    }

    // Set elements
    scene::Entity** entities = m_selector->getEntities();
    for (size_t i = 0; i < length; i++) {
        mono_array_setref(m_entities, i, locator.create(entities[i]));
    }
}

} // namespace scripting
} // namespace aderite
