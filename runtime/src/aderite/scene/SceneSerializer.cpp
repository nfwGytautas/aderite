#include "SceneSerializer.hpp"

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/Scenery.hpp"
#include "aderite/scene/Visual.hpp"
#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/scripting/LibClassLocator.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace scene {

#define SERIALIZE_LIST(rootName, objects)                \
    emitter << YAML::Key << rootName << YAML::BeginSeq;  \
    for (const auto* object : objects) {                 \
        serializer->writeUntrackedType(emitter, object); \
    }                                                    \
    emitter << YAML::EndSeq;

// TODO: Check for failed parsing
#define DESERIALIZE_LIST(rootName, type, fn)                                               \
    {                                                                                      \
        auto objects = data[rootName];                                                     \
        if (objects) {                                                                     \
            for (auto object : objects) {                                                  \
                type* parsed = static_cast<type*>(serializer->parseUntrackedType(object)); \
                fn(parsed);                                                                \
            }                                                                              \
        }                                                                                  \
    }

bool SceneSerializer::serialize(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& emitter) const {
    LOG_TRACE("[Asset] Serializing scene {0}", scene->getName());

    SERIALIZE_LIST("Visuals", scene->getVisuals());
    SERIALIZE_LIST("Scenery", scene->getScenery());

    //// Entities
    // LOG_TRACE("[Asset] Serializing entities {0}", scene->getHandle());
    // if (!this->serializeGenericList(serializer, emitter, "Entities", scene->getEntities())) {
    //    LOG_ERROR("[Asset] Failed to serialize entities for {0}", scene->getHandle());
    //    return false;
    //}
    // LOG_INFO("[Asset] {0} entities serialized", scene->getHandle());

    //// Audio sources
    // LOG_TRACE("[Asset] Serializing audio {0}", scene->getHandle());
    // if (!this->serializeAudio(scene, serializer, emitter)) {
    //    LOG_ERROR("[Asset] Failed to serialize audio for {0}", scene->getHandle());
    //    return false;
    //}
    // LOG_INFO("[Asset] {0} audio serialized", scene->getHandle());

    //// Physics
    // LOG_TRACE("[Asset] Serializing physics {0}", scene->getHandle());
    // if (!this->serializePhysics(scene, serializer, emitter)) {
    //    LOG_ERROR("[Asset] Failed to serialize physics for {0}", scene->getHandle());
    //    return false;
    //}
    // LOG_INFO("[Asset] {0} physics serialized", scene->getHandle());

    //// Pipeline
    // LOG_TRACE("[Asset] Serializing pipeline {0}", scene->getHandle());
    // emitter << YAML::Key << "Pipeline" << YAML::Value;
    // if (scene->m_pipeline != nullptr) {
    //    emitter << scene->m_pipeline->getHandle();
    //} else {
    //    emitter << YAML::Null;
    //}
    // LOG_INFO("[Asset] {0} pipeline serialized", scene->getHandle());

    //// Metadata
    // LOG_TRACE("[Asset] Serializing metadata {0}", scene->getHandle());
    // emitter << YAML::Key << "Tags" << YAML::Flow << scene->m_tags;
    // LOG_INFO("[Asset] {0} metadata serialized", scene->getHandle());

    //// Scripting
    // LOG_TRACE("[Asset] Serializing scripts {0}", scene->getHandle());
    // if (!this->serializeScripts(scene, serializer, emitter)) {
    //    LOG_ERROR("[Asset] Failed to serialize scripts for {0}", scene->getHandle());
    //    return false;
    //}
    // LOG_INFO("[Asset] {0} scripts serialized", scene->getHandle());

    // LOG_INFO("[Asset] Scene {0} serialized", scene->getHandle());

    return true;
}

bool SceneSerializer::deserialize(Scene* scene, io::Serializer* serializer, const YAML::Node& data) {
    LOG_TRACE("[Asset] Deserializing scene {0}", scene->getName());

    DESERIALIZE_LIST("Visuals", Visual, scene->addVisual);
    DESERIALIZE_LIST("Scenery", Scenery, scene->addScenery);

    //// Metadata
    // LOG_TRACE("[Asset] Deserializing {0:p} metadata", static_cast<void*>(scene));
    // if (data["Tags"]) {
    //    scene->m_tags = data["Tags"].as<std::vector<std::string>>();
    //}

    // LOG_INFO("[Asset] {0} metadata deserialized", scene->getHandle());

    //// Entities
    // LOG_TRACE("[Asset] Deserializing entities {0}", scene->getHandle());
    // if (!this->deserializeEntities(scene, serializer, data)) {
    //    LOG_ERROR("[Asset] Failed to deserialize entities for {0}", scene->getHandle());
    //    return false;
    //}
    // LOG_INFO("[Asset] {0} entities deserialized", scene->getHandle());

    //// Audio sources
    // LOG_TRACE("[Asset] Deserializing audio {0}", scene->getHandle());
    // if (!this->deserializeAudio(scene, serializer, data)) {
    //    LOG_ERROR("[Asset] Failed to deserialize audio for {0}", scene->getHandle());
    //    return false;
    //}
    // LOG_INFO("[Asset] {0} audio deserialized", scene->getHandle());

    //// Physics
    // LOG_TRACE("[Asset] Deserializing physics {0}", scene->getHandle());
    // if (!this->deserializePhysics(scene, serializer, data)) {
    //    LOG_ERROR("[Asset] Failed to deserialize physics for {0}", scene->getHandle());
    //    return false;
    //}
    // LOG_INFO("[Asset] {0} physics deserialized", scene->getHandle());

    //// Pipeline
    // LOG_TRACE("[Asset] Deserializing pipeline {0}", scene->getHandle());
    // if (!data["Pipeline"].IsNull()) {
    //    scene->setPipeline(static_cast<rendering::Pipeline*>(
    //        ::aderite::Engine::getSerializer()->getOrRead(data["Pipeline"].as<io::SerializableHandle>())));
    //}
    // LOG_INFO("[Asset] {0} pipeline deserialized", scene->getHandle());

    //// Scripting
    // LOG_TRACE("[Asset] Deserializing scripts {0}", scene->getHandle());
    // if (!this->deserializeScripts(scene, serializer, data)) {
    //    LOG_ERROR("[Asset] Failed to deserialize scripts for {0}", scene->getHandle());
    //    return false;
    //}
    // LOG_INFO("[Asset] {0} scripts deserialized", scene->getHandle());

    LOG_INFO("[Asset] Scene {0} deserialized", scene->getName());

    return true;
}

bool SceneSerializer::serializeAudio(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& out) const {
    out << YAML::Key << "Audio" << YAML::BeginMap;

    LOG_TRACE("[Asset] Serializing audio listeners for {0}", scene->getHandle());
    out << YAML::Key << "Listeners" << YAML::BeginSeq;
    for (const audio::AudioListener* listener : scene->m_audioListeners) {
        serializer->writeUntrackedType(out, listener);
    }
    out << YAML::EndSeq;

    LOG_TRACE("[Asset] Serializing audio sources for {0}", scene->getHandle());
    out << YAML::Key << "Sources" << YAML::BeginSeq;
    for (const audio::AudioSource* source : scene->m_audioSources) {
        serializer->writeUntrackedType(out, source);
    }
    out << YAML::EndSeq;

    out << YAML::EndMap;

    return true;
}

bool SceneSerializer::deserializeAudio(Scene* scene, io::Serializer* serializer, const YAML::Node& asNode) {
    auto audio = asNode["Audio"];
    if (audio) {
        auto listeners = audio["Listeners"];
        auto sources = audio["Sources"];

        if (listeners) {
            LOG_TRACE("[Asset] Deserializing audio listeners for {0}", scene->getHandle());
            for (auto listenerNode : listeners) {
                audio::AudioListener* listener = static_cast<audio::AudioListener*>(serializer->parseUntrackedType(listenerNode));
                scene->addAudioListener(listener);
            }
        } else {
            LOG_TRACE("[Asset] No audio listeners for {0}", scene->getHandle());
        }

        if (sources) {
            LOG_TRACE("[Asset] Deserializing audio sources for {0}", scene->getHandle());
            for (auto sourceNode : sources) {
                audio::AudioSource* source = static_cast<audio::AudioSource*>(serializer->parseUntrackedType(sourceNode));
                scene->addAudioSource(source);
            }
        } else {
            LOG_TRACE("[Asset] No audio sources for {0}", scene->getHandle());
        }
    }

    return true;
}

bool SceneSerializer::serializePhysics(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& out) const {
    out << YAML::Key << "Physics";
    serializer->writeUntrackedType(out, scene->getPhysicsScene());
    return true;
}

bool SceneSerializer::deserializePhysics(Scene* scene, io::Serializer* serializer, const YAML::Node& asNode) {
    auto physics = asNode["Physics"];
    if (physics) {
        serializer->fillData(scene->getPhysicsScene(), physics);
    }
    return true;
}

bool SceneSerializer::serializeScripts(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& out) const {
    out << YAML::Key << "Scripting" << YAML::BeginMap;

    LOG_TRACE("[Asset] Serializing script systems for {0}", scene->getHandle());
    out << YAML::Key << "Systems" << YAML::BeginSeq;
    for (const scripting::ScriptSystem* system : scene->m_systems) {
        // System info
        serializer->writeUntrackedType(out, system);
    }
    out << YAML::EndSeq;

    out << YAML::EndMap;
    return true;
}

bool SceneSerializer::deserializeScripts(Scene* scene, io::Serializer* serializer, const YAML::Node& asNode) {
    auto scripting = asNode["Scripting"];
    if (scripting) {
        auto systems = scripting["Systems"];

        if (systems) {
            LOG_TRACE("[Asset] Deserializing scripts systems for {0}", scene->getHandle());
            for (auto systemNode : systems) {
                scripting::ScriptSystem* system = new scripting::ScriptSystem();
                scene->addScriptSystem(system);
                serializer->fillData(system, systemNode);
            }
        } else {
            LOG_TRACE("[Asset] No script systems for {0}", scene->getHandle());
        }
    }
    return true;
}

} // namespace scene
} // namespace aderite
