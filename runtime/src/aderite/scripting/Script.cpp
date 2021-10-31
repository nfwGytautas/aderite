#include "Script.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/scripting/BehaviorWrapper.hpp"
#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

Script::Script() {}

Script::~Script() {}

std::string Script::getName() const {
    return m_name;
}

void Script::setName(const std::string& name) {
    m_name = name;

    m_behavior = ::aderite::Engine::getScriptManager()->getBehavior(m_name);
    if (m_behavior != nullptr) {
        m_instance = m_behavior->createInstance();
    }
}

void Script::update(float delta) const {
    if (m_behavior != nullptr && m_behavior->hasUpdateFunction()) {
        m_behavior->update(m_instance, delta);
    }
}

void Script::pair(scene::Entity entity) {
    if (m_behavior != nullptr && m_instance != nullptr) {
        mono_runtime_object_init(m_instance);
        m_behavior->pair(entity, m_instance);
    }
}

void Script::onTriggerEnter(scene::Entity trigger) {
    if (m_behavior != nullptr && m_behavior->hasTriggerEnterFunction()) {
        // TODO: Optimize by creating once
        MonoObject* entity = ::aderite::Engine::getScriptManager()->createEntityObject(trigger);
        m_behavior->onTriggerEnter(m_instance, entity);
    }
}

void Script::onTriggerLeave(scene::Entity trigger) {
    if (m_behavior != nullptr && m_behavior->hasTriggerLeaveFunction()) {
        // TODO: Optimize by creating once
        MonoObject* entity = ::aderite::Engine::getScriptManager()->createEntityObject(trigger);
        m_behavior->onTriggerLeave(m_instance, entity);
    }
}

void Script::onCollisionEnter(scene::Entity collision) {
    if (m_behavior != nullptr && m_behavior->hasCollisionEnterFunction()) {
        // TODO: Optimize by creating once
        MonoObject* entity = ::aderite::Engine::getScriptManager()->createEntityObject(collision);
        m_behavior->onCollisionEnter(m_instance, entity);
    }
}

void Script::onCollisionLeave(scene::Entity collision) {
    if (m_behavior != nullptr && m_behavior->hasCollisionLeaveFunction()) {
        // TODO: Optimize by creating once
        MonoObject* entity = ::aderite::Engine::getScriptManager()->createEntityObject(collision);
        m_behavior->onCollisionLeave(m_instance, entity);
    }
}

BehaviorWrapper* Script::getBehavior() const {
    return m_behavior;
}

MonoObject* Script::getInstance() const {
    return m_instance;
}

reflection::Type Script::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::SCRIPT);
}

bool Script::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    emitter << YAML::Key << "Name" << YAML::Value << m_name;

    emitter << YAML::Key << "Fields" << YAML::BeginMap;
    for (FieldWrapper* fw : m_behavior->getFields()) {
        emitter << YAML::Key << fw->getName() << YAML::Value;

        switch (fw->getType()) {
        case FieldType::Float: {
            emitter << (*(float*)unbox(fw->getValueObject(m_instance)));
            break;
        }
        case FieldType::Mesh: {
            MonoObject* mesh = nullptr;
            fw->getValue(m_instance, &mesh);
            if (mesh == nullptr) {
                emitter << YAML::Null;
            } else {
                asset::MeshAsset* meshAsset = nullptr;
                extractMesh(mesh, meshAsset);
                emitter << meshAsset->getHandle();
            }
            break;
        }
        case FieldType::Material: {
            MonoObject* material = nullptr;
            fw->getValue(m_instance, &material);
            if (material == nullptr) {
                emitter << YAML::Null;
            } else {
                asset::MaterialAsset* materialAsset = nullptr;
                extractMaterial(material, materialAsset);
                emitter << materialAsset->getHandle();
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

bool Script::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    m_name = data["Name"].as<std::string>();

    m_behavior = ::aderite::Engine::getScriptManager()->getBehavior(m_name);

    if (m_behavior == nullptr) {
        // Assembly probably changed ignore
        LOG_WARN("Script {0}, no longer exists in assembly", m_name);
        return true;
    }

    m_instance = m_behavior->createInstance();

    for (auto& fieldData : data["Fields"]) {
        FieldWrapper* fw = m_behavior->getField(fieldData.first.as<std::string>());

        if (fw != nullptr && !fieldData.second.IsNull()) {
            switch (fw->getType()) {
            case FieldType::Float: {
                float val = fieldData.second.as<float>();
                fw->setValue(m_instance, &val);
                break;
            }
            case FieldType::Mesh: {
                asset::MeshAsset* mesh = static_cast<asset::MeshAsset*>(
                    ::aderite::Engine::getSerializer()->getOrRead(fieldData.second.as<io::SerializableHandle>()));
                MonoObject* obj = ::aderite::Engine::getScriptManager()->createMeshObject(mesh);
                fw->setValue(m_instance, obj);
                break;
            }
            case FieldType::Material: {
                asset::MaterialAsset* material = static_cast<asset::MaterialAsset*>(
                    ::aderite::Engine::getSerializer()->getOrRead(fieldData.second.as<io::SerializableHandle>()));
                MonoObject* obj = ::aderite::Engine::getScriptManager()->createMaterialObject(material);
                fw->setValue(m_instance, obj);
                break;
            }
            }
        }
    }

    return true;
}

} // namespace scripting
} // namespace aderite
