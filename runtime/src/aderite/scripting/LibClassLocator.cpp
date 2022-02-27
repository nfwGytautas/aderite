#include "LibClassLocator.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/PrefabAsset.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/geometry/Geometry.hpp"
#include "aderite/scene/GameObject.hpp"
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
    findClass(image, "Aderite", "ScriptedBehavior", Behavior.Klass, result);
    findClass(image, "Aderite", "GameObject", GameObject.Klass, result);
    findClass(image, "Aderite", "Prefab", Prefab.Klass, result);
    findClass(image, "Aderite", "Transform", Transform.Klass, result);

    // Can't proceed if classes are not found
    if (result == false) {
        LOG_ERROR("[Scripting] Failed to find one of engine classes aborting");
        return false;
    }

    // Fields

    // Methods
    findMethod(GameObject.Klass, ".ctor", 1, GameObject.Ctor, result);
    findMethod(Prefab.Klass, ".ctor", 1, Prefab.Ctor, result);
    findMethod(Transform.Klass, ".ctor", 1, Transform.Ctor, result);

    if (result) {
        LOG_INFO("[Scripting] Engine classes located");
    } else {
        LOG_ERROR("[Scripting] Some engine classed couldn't be loaded");
    }

    return result;
}

FieldType LibClassLocator::getType(MonoType* type) const {
    // Check for engine classes
    MonoClass* klass = mono_type_get_class(type);

    if (klass == Prefab.Klass) {
        return FieldType::Prefab;
    }

    /*if (klass == m_mesh.Klass) {
        return FieldType::Mesh;
    } else if (klass == m_material.Klass) {
        return FieldType::Material;
    } else if (klass == m_audio.Klass) {
        return FieldType::Audio;
    } else if (klass == m_audioSource.Klass) {
        return FieldType::AudioSource;
    } else if (klass == m_geometry.Klass) {
        return FieldType::Geometry;
    }*/

    return FieldType::Null;
}

MonoObject* LibClassLocator::create(io::SerializableObject* serializable) const {
    switch (static_cast<reflection::RuntimeTypes>(serializable->getType())) {
    case reflection::RuntimeTypes::GAME_OBJECT: {
        return this->create(static_cast<scene::GameObject*>(serializable));
    }
    case reflection::RuntimeTypes::PREFAB: {
        return this->create(static_cast<asset::PrefabAsset*>(serializable));
    }
    default: {
        ADERITE_ABORT("Unknown serializable type");
        return nullptr;
    }
    }
}

MonoObject* LibClassLocator::create(scene::GameObject* gObject) const {
    void* args[1] = {&gObject};
    return this->genericInstanceCreate(GameObject.Klass, GameObject.Ctor, args);
}

MonoObject* LibClassLocator::create(scene::TransformProvider* transform) const {
    void* args[1] = {&transform};
    return this->genericInstanceCreate(Transform.Klass, Transform.Ctor, args);
}

MonoObject* LibClassLocator::create(asset::PrefabAsset* prefab) const {
    void* args[1] = {&prefab};
    return this->genericInstanceCreate(Prefab.Klass, Prefab.Ctor, args);
}

void LibClassLocator::handleException(MonoObject* exception) const {
    // TODO: Implement
    LOG_ERROR("[Scripting] EXCEPTION THROWN IN C# CODE");
}

MonoObject* LibClassLocator::genericInstanceCreate(MonoClass* klass, MonoMethod* ctor, void** args) const {
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
