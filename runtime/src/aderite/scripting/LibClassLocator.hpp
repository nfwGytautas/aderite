#pragma once

#include <mono/jit/jit.h>

#include "aderite/asset/Forward.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scripting/FieldWrapper.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Class used to locate engine classes and provide ways to instantiate them
 */
class LibClassLocator final {
public:
    struct Behavior {
        MonoClass* Klass = nullptr;
        MonoClassField* Entity = nullptr;
    };

    struct Entity {
        MonoClass* Klass = nullptr;
        MonoClassField* Scene = nullptr;
        MonoClassField* EntityHandle = nullptr;
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
     * @brief Checks if the specified class is a behavior class
     * @param klass Class to check
     * @return True if the class is or inherits from behavior class, false otherwise
     */
    bool isBehavior(MonoClass* klass) const;

    /**
     * @brief Creates a C# script Entity object from C++ entity
     * @param entity Entity from which to create
     * @return MonoObject instance
     */
    MonoObject* create(scene::Entity entity);

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

    // Class getters
    const Behavior& getBehavior() const {
        return m_behavior;
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

private:
    /**
     * @brief Handle mono exception object
     * @param exception Exception to handle
     */
    void handleException(MonoObject* exception);

    /**
     * @brief Generic asset creation function
     * @param klass Class of the asset
     * @param ctor Constructor method
     * @param args Arguments to pass to constructor
     * @return MonoObject instance
     */
    MonoObject* genericAssetCreate(MonoClass* klass, MonoMethod* ctor, void** args);

private:
    Behavior m_behavior;
    Entity m_entity;
    Mesh m_mesh;
    Material m_material;
    Audio m_audio;
    AudioSource m_audioSource;
};

} // namespace scripting
} // namespace aderite
