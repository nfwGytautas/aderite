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
#include "aderite/physics/DynamicActor.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/PhysicsScene.hpp"
#include "aderite/physics/StaticActor.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/EntitySelector.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneSerializer.hpp"
#include "aderite/scene/Transform.hpp"
#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/scripting/LibClassLocator.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
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
    if (!this->serializeAudio(scene, serializer, emitter)) {
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

    // Tags
    emitter << YAML::Key << "Tags" << YAML::Flow << scene->m_tags;

    // Scripting
    if (!this->serializeScripts(scene, serializer, emitter)) {
        return false;
    }

    return true;
}

bool SceneSerializer::deserialize(Scene* scene, io::Serializer* serializer, const YAML::Node& data) {
    // Entities
    if (!this->deserializeEntities(scene, serializer, data)) {
        return false;
    }

    // Audio sources
    if (!this->deserializeAudio(scene, serializer, data)) {
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

    // Tags
    if (data["Tags"]) {
        scene->m_tags = data["Tags"].as<std::vector<std::string>>();
    }

    // Scripting
    if (!this->deserializeScripts(scene, serializer, data)) {
        return false;
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

bool SceneSerializer::serializeAudio(const Scene* scene, const io::Serializer* serializer, YAML::Emitter& out) const {
    out << YAML::Key << "Audio" << YAML::BeginMap;

    out << YAML::Key << "Listeners" << YAML::BeginSeq;
    for (const audio::AudioListener* listener : scene->m_audioListeners) {
        serializer->writeUntrackedType(out, listener);
    }
    out << YAML::EndSeq;

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
            for (auto listenerNode : listeners) {
                audio::AudioListener* listener = static_cast<audio::AudioListener*>(serializer->parseUntrackedType(listenerNode));
                scene->addAudioListener(listener);
            }
        }

        if (sources) {
            for (auto sourceNode : sources) {
                audio::AudioSource* source = static_cast<audio::AudioSource*>(serializer->parseUntrackedType(sourceNode));
                scene->addAudioSource(source);
            }
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

    out << YAML::Key << "Selectors" << YAML::BeginSeq;
    for (const EntitySelector* selector : scene->m_entitySelectors) {
        // System info
        serializer->writeUntrackedType(out, selector);
    }
    out << YAML::EndSeq;

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
        auto selectors = scripting["Selectors"];

        if (selectors) {
            for (auto selectorNode : selectors) {
                EntitySelector* selector = static_cast<EntitySelector*>(serializer->parseUntrackedType(selectorNode));
                scene->addEntitySelector(selector);
            }
        }

        if (systems) {
            for (auto systemNode : systems) {
                scripting::ScriptSystem* system = new scripting::ScriptSystem();
                scene->addScriptSystem(system);
                serializer->fillData(system, systemNode);
            }
        }
    }
    return true;
}

} // namespace scene
} // namespace aderite
