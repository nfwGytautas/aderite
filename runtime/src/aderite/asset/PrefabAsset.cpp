#include "PrefabAsset.hpp"

#include "aderite/io/Serializer.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Scene.hpp"

namespace aderite {
namespace asset {

PrefabAsset::~PrefabAsset() {
    if (m_prototype != nullptr) {
        delete m_prototype;
    }
}

void PrefabAsset::setPrototype(scene::Entity* entity) {
    if (m_prototype != nullptr) {
        delete m_prototype;
    }

    m_prototype = entity->clone();
    m_prototype->m_scene = nullptr;
}

scene::Entity* PrefabAsset::getPrototype() const {
    return m_prototype;
}

scene::Entity* PrefabAsset::instantiate(scene::Scene* scene) {
    if (m_prototype != nullptr) {
        scene::Entity* clone = m_prototype->clone();
        scene->addEntity(clone);
        clone->m_prefab = this;
        return clone;
    }

    return nullptr;
}

reflection::Type PrefabAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::PREFAB);
}

bool PrefabAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Prototype";
    if (m_prototype != nullptr) {
        serializer->writeUntrackedType(emitter, m_prototype);
    } else {
        emitter << YAML::Null;
    }

    return true;
}

bool PrefabAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (data["Prototype"] && !data["Prototype"].IsNull()) {
        m_prototype = static_cast<scene::Entity*>(serializer->parseUntrackedType(data["Prototype"]));
    }

    return true;
}

} // namespace asset
} // namespace aderite
