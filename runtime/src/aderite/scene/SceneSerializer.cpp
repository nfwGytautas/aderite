#include "SceneSerializer.hpp"

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneSerializer.hpp"
#include "aderite/scene/Transform.hpp"
#include "aderite/scripting/ScriptList.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace scene {

bool SceneSerializer::serialize(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Entities
    if (!this->serializeEntities(scene, serializer, emitter)) {
        return false;
    }

    // Audio sources
    if (!this->serializeAudioSources(scene, serializer, emitter)) {
        return false;
    }

    // Physics
    if (!this->serializePhysics(scene, serializer, emitter)) {
        return false;
    }

    // Pipeline
    emitter << YAML::Key << "Pipeline" << YAML::Value;
    if (scene->m_pipeline != nullptr) {
        emitter << scene->m_pipeline->getHandle();
    } else {
        emitter << YAML::Null;
    }

    return true;
}

bool SceneSerializer::deserialize(Scene* scene, io::Serializer* serializer, const YAML::Node& data) {
    // Entities
    if (!this->deserializeEntities(scene, serializer, data)) {
        return false;
    }

    // Audio sources
    if (!this->deserializeAudioSources(scene, serializer, data)) {
        return false;
    }

    // Physics
    if (!this->deserializePhysics(scene, serializer, data)) {
        return false;
    }

    // Pipeline
    if (!data["Pipeline"].IsNull()) {
        scene->setPipeline(static_cast<rendering::Pipeline*>(
            ::aderite::Engine::getSerializer()->getOrRead(data["Pipeline"].as<io::SerializableHandle>())));
    }

    return true;
}

bool SceneSerializer::serializeEntities(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& out) const {
    out << YAML::Key << "Entities" << YAML::BeginSeq;
    for (const Entity* entity : scene->m_entities) {
        serializer->writeUntrackedType(out, entity);
    }
    out << YAML::EndSeq;

    return true;
}

bool SceneSerializer::deserializeEntities(Scene* scene, io::Serializer* serializer, const YAML::Node& asNode) {
    auto entities = asNode["Entities"];
    if (entities) {
        // TODO: Clear entity pool
        for (auto entityNode : entities) {
            Entity* e = static_cast<Entity*>(serializer->parseUntrackedType(entityNode));
            scene->addEntity(e);
        }
    }

    return true;
}

// void SceneSerializer::serializeEntity(const Scene* scene, YAML::Emitter& out, ConstEntity e) const {
//    out << YAML::BeginMap; // Entity
//
//    out << YAML::Key << "Entity" << YAML::Value << (entt::id_type)e;
//
//    // Meta component
//    if (e.hasComponent<MetaComponent>()) {
//        out << YAML::Key << "Meta";
//        out << YAML::BeginMap; // Meta
//
//        const MetaComponent& mc = e.getComponent<MetaComponent>();
//        out << YAML::Key << "Name" << YAML::Value << mc.Name;
//        out << YAML::Key << "Handle" << YAML::Value << mc.Handle;
//
//        out << YAML::EndMap; // Meta
//    } else {
//        LOG_ERROR("Tried to serialize Entity without MetaComponent component, this should never happen");
//        return;
//    }
//
//    // Serialize rest of components
//
//    // Camera
//    if (e.hasComponent<CameraComponent>()) {
//        out << YAML::Key << "Camera";
//        out << YAML::BeginMap; // Camera
//
//        const CameraComponent& cc = e.getComponent<CameraComponent>();
//
//        // TODO: Serialize camera
//
//        out << YAML::EndMap; // Camera
//    }
//
//    // Audio listener
//    if (e.hasComponent<AudioListenerComponent>()) {
//        out << YAML::Key << "AudioListener";
//        out << YAML::BeginMap; // AudioListener
//
//        const AudioListenerComponent& alc = e.getComponent<AudioListenerComponent>();
//
//        out << YAML::Key << "IsEnabled" << alc.IsEnabled;
//
//        out << YAML::EndMap; // AudioListener
//    }
//
//    // Scripts
//    if (e.hasComponent<ScriptsComponent>()) {
//        out << YAML::Key << "Scripts";
//        const ScriptsComponent& sc = e.getComponent<ScriptsComponent>();
//
//        // TODO: Error check
//        ::aderite::Engine::getSerializer()->writeUntrackedType(out, sc.Scripts);
//    }
//
//    out << YAML::EndMap; // Entity
//}

// Entity SceneSerializer::deserializeEntity(Scene* scene, const YAML::Node& eNode) {
//    // Read MetaComponent info
//    YAML::Node meta_node = eNode["Meta"];
//    if (!meta_node) {
//        LOG_ERROR("Tried to deserialize Entity without MetaComponent component, this should never happen");
//        return Entity(entt::null, nullptr);
//    }
//
//    MetaComponent MetaComponent;
//    MetaComponent.Name = meta_node["Name"].as<std::string>();
//    MetaComponent.Handle = meta_node["Handle"].as<size_t>();
//
//    // Create Entity
//    Entity e = scene->createEntity(MetaComponent);
//
//    // Deserialize rest of components
//
//    // Camera
//    auto cam_node = eNode["Camera"];
//    if (cam_node) {
//        auto& cc = e.addComponent<CameraComponent>();
//
//        // TODO: Deserialize camera
//    }
//
//    // Audio listener
//    auto al_node = eNode["AudioListener"];
//    if (al_node) {
//        auto& alc = e.addComponent<AudioListenerComponent>();
//
//        // TODO: Error check
//        alc.IsEnabled = al_node["IsEnabled"].as<bool>();
//    }
//
//    // Scripts
//    auto scripts = eNode["Scripts"];
//    if (scripts) {
//        if (!e.hasComponent<ScriptsComponent>()) {
//            e.addComponent<ScriptsComponent>();
//        }
//
//        auto& sc = e.getComponent<ScriptsComponent>();
//
//        // TODO: Error check
//        sc.Scripts->deserialize(::aderite::Engine::getSerializer(), scripts["Data"]);
//        sc.Scripts->pair(e);
//    }
//
//    return e;
//}

bool SceneSerializer::serializeAudioSources(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& out) const {
    out << YAML::Key << "AudioSources" << YAML::BeginSeq;
    for (const audio::AudioSource* source : scene->m_audioSources) {
        serializer->writeUntrackedType(out, source);
    }
    out << YAML::EndSeq;

    return true;
}

bool SceneSerializer::deserializeAudioSources(Scene* scene, io::Serializer* serializer, const YAML::Node& asNode) {
    auto sources = asNode["AudioSources"];
    if (sources) {
        for (auto sourceNode : sources) {
            audio::AudioSource* source = static_cast<audio::AudioSource*>(serializer->parseUntrackedType(sourceNode));
            scene->m_audioSources.push_back(source);

            // TODO: Remove this when standalone scripts are introduced
            ::aderite::Engine::getAudioController()->addSource(source);
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

} // namespace scene
} // namespace aderite
