#include "SceneSerializer.hpp"

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/ColliderListAsset.hpp"
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
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/SceneSerializer.hpp"
#include "aderite/scene/components/Actors.hpp"
#include "aderite/scene/components/Audio.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/scene/components/Meta.hpp"
#include "aderite/scene/components/Transform.hpp"
#include "aderite/scripting/ScriptList.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace scene {

bool SceneSerializer::serialize(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& emitter) const {
    // Audio sources
    if (!this->serializeAudioSources(scene, serializer, emitter)) {
        return false;
    }

    // Pipeline
    emitter << YAML::Key << "Pipeline" << YAML::Value;
    if (scene->m_pipeline != nullptr) {
        emitter << scene->m_pipeline->getHandle();
    } else {
        emitter << YAML::Null;
    }
    
    // Entities
    emitter << YAML::Key << "Entities" << YAML::BeginSeq;
    scene->m_registry.each([&](auto entity_id) {
        ConstEntity e = ConstEntity(entity_id, scene);

        if (!e) {
            return;
        }

        // TODO: Error check
        this->serializeEntity(scene, emitter, e);
    });
    emitter << YAML::EndSeq; // Entities

    return true;
}

bool SceneSerializer::deserialize(Scene* scene, io::Serializer* serializer, const YAML::Node& data) {
    // Audio sources
    if (!this->deserializeAudioSources(scene, serializer, data)) {
        return false;
    }
    
    // Pipeline
    if (!data["Pipeline"].IsNull()) {
        scene->setPipeline(static_cast<rendering::Pipeline*>(
            ::aderite::Engine::getSerializer()->getOrRead(data["Pipeline"].as<io::SerializableHandle>())));
    }

    // Entities
    auto entities = data["Entities"];
    if (entities) {
        for (auto Entity : entities) {
            // Error check
            this->deserializeEntity(scene, Entity);
        }
    }

    return true;
}

void SceneSerializer::serializeEntity(const Scene* scene, YAML::Emitter& out, ConstEntity e) const {
    out << YAML::BeginMap; // Entity

    out << YAML::Key << "Entity" << YAML::Value << (entt::id_type)e;

    // Meta component
    if (e.hasComponent<MetaComponent>()) {
        out << YAML::Key << "Meta";
        out << YAML::BeginMap; // Meta

        const MetaComponent& mc = e.getComponent<MetaComponent>();
        out << YAML::Key << "Name" << YAML::Value << mc.Name;

        out << YAML::EndMap; // Meta
    } else {
        LOG_ERROR("Tried to serialize Entity without MetaComponent component, this should never happen");
        return;
    }

    // Serialize rest of components

    // Transform
    if (e.hasComponent<TransformComponent>()) {
        out << YAML::Key << "Transform";
        out << YAML::BeginMap; // Transform

        const TransformComponent& tc = e.getComponent<TransformComponent>();
        out << YAML::Key << "Position" << YAML::Value << tc.Position;
        out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
        out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

        out << YAML::EndMap; // Transform
    }

    // Mesh Renderer
    if (e.hasComponent<MeshRendererComponent>()) {
        out << YAML::Key << "MeshRenderer";
        out << YAML::BeginMap; // MeshRenderer

        const MeshRendererComponent& mrc = e.getComponent<MeshRendererComponent>();

        if (mrc.MeshHandle) {
            out << YAML::Key << "Mesh" << YAML::Value << mrc.MeshHandle->getHandle();
        }

        if (mrc.MaterialHandle) {
            out << YAML::Key << "Material" << YAML::Value << mrc.MaterialHandle->getHandle();
        }

        out << YAML::EndMap; // MeshRenderer
    }

    // Camera
    if (e.hasComponent<CameraComponent>()) {
        out << YAML::Key << "Camera";
        out << YAML::BeginMap; // Camera

        const CameraComponent& cc = e.getComponent<CameraComponent>();

        // TODO: Serialize camera

        out << YAML::EndMap; // Camera
    }

    // Dynamic actor
    if (e.hasComponent<DynamicActor>()) {
        out << YAML::Key << "DynamicActor";

        const DynamicActor& dac = e.getComponent<DynamicActor>();
        ::aderite::Engine::getSerializer()->writeUntrackedType(out, dac.Actor);
    }

    // Static actor
    if (e.hasComponent<StaticActor>()) {
        out << YAML::Key << "StaticActor";

        const StaticActor& dac = e.getComponent<StaticActor>();
        ::aderite::Engine::getSerializer()->writeUntrackedType(out, dac.Actor);
    }

    // Colliders
    if (e.hasComponent<CollidersComponent>()) {
        out << YAML::Key << "Colliders";
        out << YAML::BeginMap; // Colliders
        const CollidersComponent& cc = e.getComponent<CollidersComponent>();

        if (cc.Colliders) {
            out << YAML::Key << "ID" << YAML::Value << cc.Colliders->getHandle();
        }
        out << YAML::EndMap;
    }

    // Audio listener
    if (e.hasComponent<AudioListenerComponent>()) {
        out << YAML::Key << "AudioListener";
        out << YAML::BeginMap; // AudioListener

        const AudioListenerComponent& alc = e.getComponent<AudioListenerComponent>();

        out << YAML::Key << "IsEnabled" << alc.IsEnabled;

        out << YAML::EndMap; // AudioListener
    }

    // Scripts
    if (e.hasComponent<ScriptsComponent>()) {
        out << YAML::Key << "Scripts";
        const ScriptsComponent& sc = e.getComponent<ScriptsComponent>();

        // TODO: Error check
        ::aderite::Engine::getSerializer()->writeUntrackedType(out, sc.Scripts);
    }

    out << YAML::EndMap; // Entity
}

Entity SceneSerializer::deserializeEntity(Scene* scene, const YAML::Node& eNode) {
    // Read MetaComponent info
    YAML::Node meta_node = eNode["Meta"];
    if (!meta_node) {
        LOG_ERROR("Tried to deserialize Entity without MetaComponent component, this should never happen");
        return Entity(entt::null, nullptr);
    }

    MetaComponent MetaComponent;
    MetaComponent.Name = meta_node["Name"].as<std::string>();

    // Create Entity
    Entity e = scene->createEntity(MetaComponent);

    // Deserialize rest of components

    // Transform
    auto transform_node = eNode["Transform"];
    if (transform_node) {
        auto& tc = e.addComponent<TransformComponent>();
        tc.Position = transform_node["Position"].as<glm::vec3>();
        tc.Rotation = transform_node["Rotation"].as<glm::quat>();
        tc.Scale = transform_node["Scale"].as<glm::vec3>();
    }

    // Mesh Renderer
    auto mr_node = eNode["MeshRenderer"];
    if (mr_node) {
        auto& mrc = e.addComponent<MeshRendererComponent>();

        if (mr_node["Mesh"]) {
            const io::SerializableHandle handle = mr_node["Mesh"].as<io::SerializableHandle>();
            mrc.MeshHandle = static_cast<asset::MeshAsset*>(::aderite::Engine::getSerializer()->getOrRead(handle));
            ADERITE_DYNAMIC_ASSERT(mrc.MeshHandle != nullptr, "Tried to use a deleted asset");
        }

        if (mr_node["Material"]) {
            const io::SerializableHandle handle = mr_node["Material"].as<io::SerializableHandle>();
            mrc.MaterialHandle = static_cast<asset::MaterialAsset*>(::aderite::Engine::getSerializer()->getOrRead(handle));
            ADERITE_DYNAMIC_ASSERT(mrc.MaterialHandle != nullptr, "Tried to use a deleted asset");
        }
    }

    // Camera
    auto cam_node = eNode["Camera"];
    if (cam_node) {
        auto& cc = e.addComponent<CameraComponent>();

        // TODO: Deserialize camera
    }

    // Dynamic actor
    auto da_node = eNode["DynamicActor"];
    if (da_node) {
        auto& dac = e.addComponent<DynamicActor>();

        // TODO: Error check
        ::aderite::Engine::getSerializer()->fillData(dac.Actor, da_node);
    }

    // Static actor
    auto sa_node = eNode["StaticActor"];
    if (sa_node) {
        auto& sac = e.addComponent<StaticActor>();

        // TODO: Error check
        ::aderite::Engine::getSerializer()->fillData(sac.Actor, sa_node);
    }

    // Colliders
    auto colliders = eNode["Colliders"];
    if (colliders) {
        auto& cc = e.addComponent<CollidersComponent>();

        // TODO: Error check
        if (colliders["ID"]) {
            const io::SerializableHandle handle = colliders["ID"].as<io::SerializableHandle>();
            cc.Colliders = static_cast<asset::ColliderListAsset*>(::aderite::Engine::getSerializer()->getOrRead(handle));
            ADERITE_DYNAMIC_ASSERT(cc.Colliders != nullptr, "Tried to use a deleted asset");
        }
    }

    // Audio listener
    auto al_node = eNode["AudioListener"];
    if (al_node) {
        auto& alc = e.addComponent<AudioListenerComponent>();

        // TODO: Error check
        alc.IsEnabled = al_node["IsEnabled"].as<bool>();
    }

    // Scripts
    auto scripts = eNode["Scripts"];
    if (scripts) {
        if (!e.hasComponent<ScriptsComponent>()) {
            e.addComponent<ScriptsComponent>();
        }

        auto& sc = e.getComponent<ScriptsComponent>();

        // TODO: Error check
        sc.Scripts->deserialize(::aderite::Engine::getSerializer(), scripts["Data"]);
        sc.Scripts->pair(e);
    }

    return e;
}

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

} // namespace scene
} // namespace aderite
