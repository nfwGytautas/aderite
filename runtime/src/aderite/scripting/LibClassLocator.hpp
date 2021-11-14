#pragma once

#include <mono/jit/jit.h>

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/io/Forward.hpp"
#include "aderite/physics/PhysicsEventList.hpp"
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

    /**
     * @brief Create a C# script instance object from generic C++ serializable object
     * @param serializable Serializable object
     * @return MonoObject instance
    */
    MonoObject* create(io::ISerializable* serializable);

    /**
     * @brief Creates a C# script Entity object from C++ entity
     * @param entity Entity from which to create
     * @return MonoObject instance
     */
    MonoObject* create(scene::Entity* entity);

    /**
     * @brief Creates a C# mesh object from C++ asset
     * @param mesh Mesh from which to create
     * @return MonoObject instance
     */
    MonoObject* create(asset::MeshAsset* mesh);

    /**
     * @brief Creates a C# material object from C++ asset
     * @param material Material from which to create
     * @return MonoObject instance
     */
    MonoObject* create(asset::MaterialAsset* material);

    /**
     * @brief Creates a C# audio object from C++ asset
     * @param audio Audio from which to create
     * @return MonoObject instance
     */
    MonoObject* create(asset::AudioAsset* audio);

    /**
     * @brief Creates a C# audio source object from C++
     * @param source Source from which to create
     * @return MonoObject instance
     */
    MonoObject* create(audio::AudioSource* source);

    /**
     * @brief Creates a C# trigger event object from C++
     * @param triggerEvent TriggerEvent object
     * @return MonoObject instance
     */
    MonoObject* create(const physics::TriggerEvent& triggerEvent);

    /**
     * @brief Creates a C# collision event object from C++
     * @param collisionEvent CollisionEvent object
     * @return MonoObject instance
     */
    MonoObject* create(const physics::CollisionEvent& collisionEvent);

    // Class getters
    const ScriptSystem& getScriptSystem() const {
        return m_system;
    }

    const Entity& getEntity() const {
        return m_entity;
    }

    const Mesh& getMesh() const {
        return m_mesh;
    }

    const Material& getMaterial() const {
        return m_material;
    }

    const Audio& getAudio() const {
        return m_audio;
    }

    const AudioSource& getAudioSource() const {
        return m_audioSource;
    }

    const TriggerEvent& getTriggerEvent() const {
        return m_triggerEvent;
    }

    const CollisionEvent& getCollisionEvent() const {
        return m_collisionEvent;
    }

private:
    /**
     * @brief Handle mono exception object
     * @param exception Exception to handle
     */
    void handleException(MonoObject* exception);

    /**
     * @brief Generic instanced object creation function
     * @param klass Class of the asset
     * @param ctor Constructor method
     * @param args Arguments to pass to constructor
     * @return MonoObject instance
     */
    MonoObject* genericInstanceCreate(MonoClass* klass, MonoMethod* ctor, void** args);

private:
    ScriptSystem m_system;
    Entity m_entity;
    Mesh m_mesh;
    Material m_material;
    Audio m_audio;
    AudioSource m_audioSource;
    TriggerEvent m_triggerEvent;
    CollisionEvent m_collisionEvent;
};

} // namespace scripting
} // namespace aderite
