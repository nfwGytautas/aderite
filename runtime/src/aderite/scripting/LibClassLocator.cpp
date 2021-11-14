#include "LibClassLocator.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

void findClass(MonoImage* image, const char* nspace, const char* name, MonoClass*& klass, bool& result) {
    klass = mono_class_from_name(image, nspace, name);
    if (klass == nullptr) {
        LOG_ERROR("[Scripting] Failed to find {0} in namespace {1} class", name, nspace);
        result = false;
    }
}

void findField(MonoClass* klass, const char* name, MonoClassField*& field, bool& result) {
    field = mono_class_get_field_from_name(klass, name);
    if (field == nullptr) {
        LOG_ERROR("[Scripting] Failed to find {0} field in {1}", name, mono_class_get_name(klass));
        result = false;
    }
}

void findMethod(MonoClass* klass, const char* name, int paramCount, MonoMethod*& method, bool& result) {
    method = mono_class_get_method_from_name(klass, name, paramCount);
    if (method == nullptr) {
        LOG_ERROR("[Scripting] Failed to find {0} method in {1}", name, mono_class_get_name(klass));
        result = false;
    }
}

bool LibClassLocator::locate(MonoImage* image) {
    LOG_TRACE("[Scripting] Locating engine classes in {0:p}", static_cast<void*>(image));

    bool result = true;

    // Classes
    findClass(image, "Aderite", "ScriptSystem", m_system.Klass, result);
    findClass(image, "Aderite", "Entity", m_entity.Klass, result);
    findClass(image, "Aderite", "Mesh", m_mesh.Klass, result);
    findClass(image, "Aderite", "Material", m_material.Klass, result);
    findClass(image, "Aderite", "Audio", m_audio.Klass, result);
    findClass(image, "Aderite", "AudioSource", m_audioSource.Klass, result);
    findClass(image, "Aderite", "TriggerEvent", m_triggerEvent.Klass, result);
    findClass(image, "Aderite", "CollisionEvent", m_collisionEvent.Klass, result);

    // Can't proceed if classes are not found
    if (result == false) {
        LOG_ERROR("[Scripting] Failed to find one of engine classes aborting");
        return false;
    }

    // Fields

    // Methods
    findMethod(m_entity.Klass, ".ctor", 1, m_entity.Ctor, result);
    findMethod(m_mesh.Klass, ".ctor", 1, m_mesh.Ctor, result);
    findMethod(m_material.Klass, ".ctor", 1, m_material.Ctor, result);
    findMethod(m_audio.Klass, ".ctor", 1, m_audio.Ctor, result);
    findMethod(m_audioSource.Klass, ".ctor", 1, m_audioSource.Ctor, result);
    findMethod(m_triggerEvent.Klass, ".ctor", 2, m_triggerEvent.Ctor, result);
    findMethod(m_collisionEvent.Klass, ".ctor", 2, m_collisionEvent.Ctor, result);

    if (result) {
        LOG_INFO("[Scripting] Engine classed located");
    } else {
        LOG_ERROR("[Scripting] Some engine classed couldn't be loaded");
    }

    return result;
}

FieldType LibClassLocator::getType(MonoType* type) const {
    // Check for engine classes
    MonoClass* klass = mono_type_get_class(type);

    if (klass == m_mesh.Klass) {
        return FieldType::Mesh;
    } else if (klass == m_material.Klass) {
        return FieldType::Material;
    } else if (klass == m_audio.Klass) {
        return FieldType::Audio;
    } else if (klass == m_system.Klass) {
        return FieldType::System;
    } else if (klass == m_audioSource.Klass) {
        return FieldType::AudioSource;
    }

    return FieldType::Null;
}

bool LibClassLocator::isSystem(MonoClass* klass) const {
    if (klass == m_system.Klass) {
        // Class is behavior
        return true;
    }

    if (mono_class_get_parent(klass) == m_system.Klass) {
        // Parent is behavior
        return true;
    }

    return false;
}

MonoObject* LibClassLocator::create(io::ISerializable* serializable) {
    switch (static_cast<reflection::RuntimeTypes>(serializable->getType())) {
    case reflection::RuntimeTypes::ENTITY: {
        return this->create(static_cast<scene::Entity*>(serializable));
    }
    case reflection::RuntimeTypes::MESH: {
        return this->create(static_cast<asset::MeshAsset*>(serializable));
    }
    case reflection::RuntimeTypes::MATERIAL: {
        return this->create(static_cast<asset::MaterialAsset*>(serializable));
    }
    case reflection::RuntimeTypes::AUDIO: {
        return this->create(static_cast<asset::AudioAsset*>(serializable));
    }
    case reflection::RuntimeTypes::AUDIO_SOURCE: {
        return this->create(static_cast<audio::AudioSource*>(serializable));
    }
    default: {
        LOG_ERROR("[Scripting] Unknown implementation for creating a {0} serializable", serializable->getType());
        return nullptr;
    }
    }
}

MonoObject* LibClassLocator::create(scene::Entity* entity) {
    void* args[1] = {&entity};
    return this->genericInstanceCreate(m_entity.Klass, m_entity.Ctor, args);
}

MonoObject* LibClassLocator::create(asset::MeshAsset* mesh) {
    void* args[1] = {&mesh};
    return this->genericInstanceCreate(m_mesh.Klass, m_mesh.Ctor, args);
}

MonoObject* LibClassLocator::create(asset::MaterialAsset* material) {
    void* args[1] = {&material};
    return this->genericInstanceCreate(m_material.Klass, m_material.Ctor, args);
}

MonoObject* LibClassLocator::create(asset::AudioAsset* audio) {
    void* args[1] = {&audio};
    return this->genericInstanceCreate(m_audio.Klass, m_audio.Ctor, args);
}

MonoObject* LibClassLocator::create(audio::AudioSource* source) {
    void* args[1] = {&source};
    return this->genericInstanceCreate(m_audioSource.Klass, m_audioSource.Ctor, args);
}

MonoObject* LibClassLocator::create(const physics::TriggerEvent& triggerEvent) {
    void* args[2] = {create(triggerEvent.Actor->getEntity()), create(triggerEvent.Trigger->getEntity())};
    return this->genericInstanceCreate(m_triggerEvent.Klass, m_triggerEvent.Ctor, args);
}

MonoObject* LibClassLocator::create(const physics::CollisionEvent& collisionEvent) {
    void* args[2] = {create(collisionEvent.Actor1->getEntity()), create(collisionEvent.Actor2->getEntity())};
    return this->genericInstanceCreate(m_collisionEvent.Klass, m_collisionEvent.Ctor, args);
}

void LibClassLocator::handleException(MonoObject* exception) {
    // TODO: Implement
    LOG_ERROR("[Scripting] EXCEPTION THROWN IN C# CODE");
}

MonoObject* LibClassLocator::genericInstanceCreate(MonoClass* klass, MonoMethod* ctor, void** args) {
    // Create object
    MonoObject* object = mono_object_new(::aderite::Engine::getScriptManager()->getDomain(), klass);
    MonoObject* ex = nullptr;

    // Try to invoke constructor
    mono_runtime_invoke(ctor, object, args, &ex);

    // Handle exception if there is any
    if (ex != nullptr) {
        this->handleException(ex);
    }

    // Return instance
    return object;
}

} // namespace scripting
} // namespace aderite
