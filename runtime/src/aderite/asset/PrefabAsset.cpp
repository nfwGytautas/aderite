#include "PrefabAsset.hpp"

#include "aderite/rendering/Renderable.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace asset {

PrefabAsset::PrefabAsset() {}

PrefabAsset::PrefabAsset(scene::GameObject* gObject) {
    this->setName(gObject->getName());

    scene::TransformProvider* const transform = gObject->getTransform();
    rendering::Renderable* const renderable = gObject->getRenderable();
    physics::PhysXActor* const actor = gObject->getActor();
    scene::Camera* const camera = gObject->getCamera();
    audio::AudioListener* const listener = gObject->getAudioListener();
    audio::AudioSource* const source = gObject->getAudioSource();

    if (renderable != nullptr) {
        m_renderable = new rendering::RenderableData();
        *m_renderable = gObject->getRenderable()->getData();
    }
}

PrefabAsset::~PrefabAsset() {
    LOG_TRACE("[Asset] Destroying {0}", this->getHandle());

    delete m_renderable;
}

scene::GameObject* PrefabAsset::instantiate(scene::Scene* scene) const {
    static size_t nextId = 0;
    scene::GameObject* go = new scene::GameObject(scene, this->getName() + " " + std::to_string(nextId++));

    if (m_renderable != nullptr) {
        go->addRenderable()->getData() = *m_renderable;
    }

    return go;
}

void PrefabAsset::load(const io::Loader* loader) {
    LOG_TRACE("[Asset] Loading {0}", this->getName());
}

void PrefabAsset::unload() {
    LOG_TRACE("[Asset] Unloading {0}", this->getName());
}

bool PrefabAsset::needsLoading() const {
    return false;
}

reflection::Type PrefabAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::PREFAB);
}

bool PrefabAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (m_renderable != nullptr) {
        if (!m_renderable->serialize(serializer, emitter)) {
            return false;
        }
    }

    return true;
}

bool PrefabAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    // Create component instances
    {
        const YAML::Node& componentNode = data["Renderable"];
        if (componentNode && !componentNode.IsNull()) {
            m_renderable = new rendering::RenderableData();
            if (!m_renderable->deserialize(serializer, data)) {
                return false;
            }
        }
    }

    return true;
}

} // namespace asset
} // namespace aderite
