#pragma once

#include "aderite/io/Serializer.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Utility class for serialzing scenes
 */
class SceneSerializer final {
public:
    /**
     * @brief Serializes the scene using YAML format
     * @param scene Scene to serialize
     * @param serializer Serializer instance
     * @param emitter Emitter where to store
     * @return True if serialized, false otherwise
     */
    bool serialize(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& emitter) const;

    /**
     * @brief Deserializes the scene from YAML source
     * @param scene Scene to deserialize
     * @param serializer Serializer instance
     * @param data Data where to read from
     * @return True if deserialized, false otherwise
     */
    bool deserialize(Scene* scene, io::Serializer* serializer, const YAML::Node& data);

private:
    /**
     * @brief Serialize entities to the specified emitter
     * @param scene Scene to serialize
     * @param serializer Serializer instance
     * @param out Emitter to serialize into
     * @return True if serialized, false otherwise
     */
    bool serializeEntities(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& out) const;

    /**
     * @brief Deserialize entities
     * @param scene Scene to deserialize
     * @param serializer Serializer instance
     * @param asNode Audio source data node
     * @return True if deserialized, false otherwise
     */
    bool deserializeEntities(Scene* scene, io::Serializer* serializer, const YAML::Node& asNode);

    /**
     * @brief Serialize audio sources to the specified emitter
     * @param scene Scene to serialize
     * @param serializer Serializer instance
     * @param out Emitter to serialize into
     * @return True if serialized, false otherwise
     */
    bool serializeAudioSources(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& out) const;

    /**
     * @brief Deserialize audio sources
     * @param scene Scene to deserialize
     * @param serializer Serializer instance
     * @param asNode Audio source data node
     * @return True if deserialized, false otherwise
     */
    bool deserializeAudioSources(Scene* scene, io::Serializer* serializer, const YAML::Node& asNode);

    /**
     * @brief Serialize physics actor to the specified emitter
     * @param scene Scene to serialize
     * @param serializer Serializer instance
     * @param out Emitter to serialize into
     * @return True if serialized, false otherwise
     */
    bool serializePhysics(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& out) const;

    /**
     * @brief Deserialize physics actors
     * @param scene Scene to deserialize
     * @param serializer Serializer instance
     * @param asNode Audio source data node
     * @return True if deserialized, false otherwise
     */
    bool deserializePhysics(Scene* scene, io::Serializer* serializer, const YAML::Node& asNode);

    /**
     * @brief Serialize scripting related information to the specified emitter
     * @param scene Scene to serialize
     * @param serializer Serializer instance
     * @param out Emitter to serialize into
     * @return True if serialized, false otherwise
     */
    bool serializeScripts(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& out) const;

    /**
     * @brief Deserialize scripting related information
     * @param scene Scene to deserialize
     * @param serializer Serializer instance
     * @param asNode Audio source data node
     * @return True if deserialized, false otherwise
     */
    bool deserializeScripts(Scene* scene, io::Serializer* serializer, const YAML::Node& asNode);
};

} // namespace scene
} // namespace aderite
