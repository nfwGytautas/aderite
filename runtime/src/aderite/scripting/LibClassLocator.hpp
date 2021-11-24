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
    struct ScriptSystem {
        MonoClass* Klass = nullptr;
    };

    struct Entity {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    };

    struct Mesh {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    };

    struct Material {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    };

    struct Audio {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    };

    struct AudioSource {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    };

    struct TriggerEvent {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    };

    struct CollisionEvent {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    };

    struct RaycastHit {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    };

    struct Prefab {
        MonoClass* Klass = nullptr;
        MonoMethod* Ctor = nullptr;
    };

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

    /**
     * @brief Checks if the specified class is a system class
     * @param klass Class to check
     * @return True if the class is or inherits from system class, false otherwise
     */
    bool isSystem(MonoClass* klass) const;

    // ====================================================================================
    // Object creators
    // ====================================================================================

    /**
     * @brief Create a C# script instance object from generic C++ serializable object
     * @param serializable Serializable object
     * @return MonoObject instance
     */
    MonoObject* create(io::ISerializable* serializable) const;

    /**
     * @brief Creates a C# script Entity object from C++ entity
     * @param entity Entity from which to create
     * @return MonoObject instance
     */
    MonoObject* create(scene::Entity* entity) const;

    /**
     * @brief Creates a C# mesh object from C++ asset
     * @param mesh Mesh from which to create
     * @return MonoObject instance
     */
    MonoObject* create(asset::MeshAsset* mesh) const;

    /**
     * @brief Creates a C# material object from C++ asset
     * @param material Material from which to create
     * @return MonoObject instance
     */
    MonoObject* create(asset::MaterialAsset* material) const;

    /**
     * @brief Creates a C# audio object from C++ asset
     * @param audio Audio from which to create
     * @return MonoObject instance
     */
    MonoObject* create(asset::AudioAsset* audio) const;

    /**
     * @brief Creates a C# audio source object from C++
     * @param source Source from which to create
     * @return MonoObject instance
     */
    MonoObject* create(audio::AudioSource* source) const;

    /**
     * @brief Creates a C# trigger event object from C++
     * @param triggerEvent TriggerEvent object
     * @return MonoObject instance
     */
    MonoObject* create(const physics::TriggerEvent& triggerEvent) const;

    /**
     * @brief Creates a C# collision event object from C++
     * @param collisionEvent CollisionEvent object
     * @return MonoObject instance
     */
    MonoObject* create(const physics::CollisionEvent& collisionEvent) const;

    /**
     * @brief Creates a C# raycast hit object from C++
     * @param hit RaycastHit object
     * @return MonoObject instance
     */
    MonoObject* create(const physics::RaycastHit& hit) const;

    /**
     * @brief Creates a C# prefab object from C++
     * @param prefab Prefab object
     * @return MonoObject instance
     */
    MonoObject* create(asset::PrefabAsset* prefab) const;

    // ====================================================================================
    // Class getters
    // ====================================================================================

    /**
     * @brief Returns the script system mono information struct
     */
    const ScriptSystem& getScriptSystem() const;

    /**
     * @brief Returns the entity mono information struct
     */
    const Entity& getEntity() const;

    /**
     * @brief Returns the mesh mono information struct
     */
    const Mesh& getMesh() const;

    /**
     * @brief Returns the material mono information struct
     */
    const Material& getMaterial() const;

    /**
     * @brief Returns the audio mono information struct
     */
    const Audio& getAudio() const;

    /**
     * @brief Returns the audio source mono information struct
     */
    const AudioSource& getAudioSource() const;

    /**
     * @brief Returns the trigger evnt mono information struct
     */
    const TriggerEvent& getTriggerEvent() const;

    /**
     * @brief Returns the collision event mono information struct
     */
    const CollisionEvent& getCollisionEvent() const;

    /**
     * @brief Returns the raycast hit mono information struct
     */
    const RaycastHit& getRaycastHit() const;

    /**
     * @brief Returns the prefab mono information struct
     */
    const Prefab& getPrefab() const;

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

private:
    ScriptSystem m_system;
    Entity m_entity;
    Mesh m_mesh;
    Material m_material;
    Audio m_audio;
    AudioSource m_audioSource;
    TriggerEvent m_triggerEvent;
    CollisionEvent m_collisionEvent;
    RaycastHit m_raycastHit;
    Prefab m_prefab;
};

} // namespace scripting
} // namespace aderite
