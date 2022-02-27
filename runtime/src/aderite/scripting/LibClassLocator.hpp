#pragma once

#include <mono/jit/jit.h>

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/io/Forward.hpp"
#include "aderite/physics/PhysicsEventList.hpp"
#include "aderite/physics/PhysicsSceneQuery.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/FieldType.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Class used to locate engine classes and provide ways to instantiate them
 */
class LibClassLocator final {
public:
    struct {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    } Behavior;

    struct {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    } GameObject;

    struct {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    } Prefab;

    struct {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    } Transform;

public:
    /**
     * @brief Locates engine classes from the specified image
     * @param image Image to locate in
     * @return True if all classes found, false otherwise
     */
    bool locate(MonoImage* image);

    /**
     * @brief Check if the specified type is part of the script lib and returns it FieldType representation
     * @param type Type to check
     * @return FieldType value
     */
    FieldType getType(MonoType* type) const;

    // ====================================================================================
    // Object creators
    // ====================================================================================

    /**
     * @brief Create a C# script instance object from generic C++ serializable object
     * @param serializable Serializable object
     * @return MonoObject instance
     */
    MonoObject* create(io::SerializableObject* serializable) const;

    /**
     * @brief Creates a C# game object from C++ asset
     * @param gObject Game object from which to create
     * @return MonoObject instance
     */
    MonoObject* create(scene::GameObject* gObject) const;

    /**
     * @brief Creates a C# transform from C++ asset
     * @param Transform Transform object from which to create
     * @return MonoObject instance
     */
    MonoObject* create(scene::TransformProvider* transform) const;

    /**
     * @brief Creates a C# prefab from C++ asset
     * @param prefab Prefab from which to create
     * @return MonoObject instance
     */
    MonoObject* create(asset::PrefabAsset* prefab) const;

private:
    /**
     * @brief Handle mono exception object
     * @param exception Exception to handle
     */
    void handleException(MonoObject* exception) const;

    /**
     * @brief Generic instanced object creation function
     * @param klass Class of the asset
     * @param ctor Constructor method
     * @param args Arguments to pass to constructor
     * @return MonoObject instance
     */
    MonoObject* genericInstanceCreate(MonoClass* klass, MonoMethod* ctor, void** args) const;
};

} // namespace scripting
} // namespace aderite
