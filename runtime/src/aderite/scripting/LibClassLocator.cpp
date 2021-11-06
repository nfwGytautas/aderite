#include "LibClassLocator.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

void findClass(MonoImage* image, const char* nspace, const char* name, MonoClass*& klass, bool& result) {
    klass = mono_class_from_name(image, nspace, name);
    if (klass == nullptr) {
        LOG_ERROR("Failed to find {0} in namespace {1} class", name, nspace);
        result = false;
    }
}

void findField(MonoClass* klass, const char* name, MonoClassField*& field, bool& result) {
    field = mono_class_get_field_from_name(klass, name);
    if (field == nullptr) {
        LOG_ERROR("Failed to find {0} field in {1}", name, mono_class_get_name(klass));
        result = false;
    }
}

void findMethod(MonoClass* klass, const char* name, int paramCount, MonoMethod*& method, bool& result) {
    method = mono_class_get_method_from_name(klass, name, paramCount);
    if (method == nullptr) {
        LOG_ERROR("Failed to find {0} method in {1}", name, mono_class_get_name(klass));
        result = false;
    }
}

bool LibClassLocator::locate(MonoImage* image) {
    bool result = true;

    // Classes
    findClass(image, "Aderite", "ScriptedBehavior", m_behavior.Klass, result);
    findClass(image, "Aderite", "Entity", m_entity.Klass, result);
    findClass(image, "Aderite", "Mesh", m_mesh.Klass, result);
    findClass(image, "Aderite", "Material", m_material.Klass, result);
    findClass(image, "Aderite", "Audio", m_audio.Klass, result);

    // Can't procede if classes are not found
    if (result == false) {
        LOG_ERROR("Failed to find one of engine classes aborting");
        return false;
    }

    // Fields
    findField(m_behavior.Klass, "Entity", m_behavior.Entity, result);
    findField(m_entity.Klass, "Scene", m_entity.Scene, result);
    findField(m_entity.Klass, "EntityHandle", m_entity.EntityHandle, result);

    // Methods
    findMethod(m_mesh.Klass, ".ctor", 1, m_mesh.Ctor, result);
    findMethod(m_material.Klass, ".ctor", 1, m_material.Ctor, result);
    findMethod(m_audio.Klass, ".ctor", 1, m_audio.Ctor, result);

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
    } else if (klass == m_behavior.Klass) {
        return FieldType::Behavior;
    }

    return FieldType::Null;
}

bool LibClassLocator::isBehavior(MonoClass* klass) const {
    if (klass == m_behavior.Klass) {
        // Class is behavior
        return true;
    }

    if (mono_class_get_parent(klass) == m_behavior.Klass) {
        // Parent is behavior
        return true;
    }

    return false;
}

MonoObject* LibClassLocator::create(scene::Entity entity) {
    MonoObject* object = mono_object_new(::aderite::Engine::getScriptManager()->getDomain(), m_entity.Klass);
    entt::entity e = entity.getHandle();
    scene::Scene* scene = entity.getScene();

    mono_runtime_object_init(object);

    mono_field_set_value(object, m_entity.Scene, &scene);
    mono_field_set_value(object, m_entity.EntityHandle, &e);

    return object;
}

MonoObject* LibClassLocator::create(asset::MeshAsset* mesh) {
    void* args[1] = {&mesh};
    return this->genericAssetCreate(m_mesh.Klass, m_mesh.Ctor, args);
}

MonoObject* LibClassLocator::create(asset::MaterialAsset* material) {
    void* args[1] = {&material};
    return this->genericAssetCreate(m_material.Klass, m_material.Ctor, args);
}

MonoObject* LibClassLocator::create(asset::AudioAsset* audio) {
    void* args[1] = {&audio};
    return this->genericAssetCreate(m_audio.Klass, m_audio.Ctor, args);
}

void LibClassLocator::handleException(MonoObject* exception) {
    // TODO: Implement
}

MonoObject* LibClassLocator::genericAssetCreate(MonoClass* klass, MonoMethod* ctor, void** args) {
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
