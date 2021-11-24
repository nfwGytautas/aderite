#include "TagSelector.hpp"

#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scene {

void TagSelector::addTag(size_t tag) {
    LOG_TRACE("[Scene] Added tag {0:b} to selector {1}", tag, this->getName());
    m_tags = m_tags | tag;
}

void TagSelector::removeTag(size_t tag) {
    LOG_TRACE("[Scene] Removed tag {0:b} from selector {1}", tag, this->getName());
    m_tags = m_tags & ~tag;
}

bool TagSelector::hasTag(size_t tag) const {
    return m_tags & tag;
}

size_t TagSelector::getTags() const {
    return m_tags;
}

reflection::Type TagSelector::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::TAG_SELECTOR);
}

bool TagSelector::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    if (!EntitySelector::serialize(serializer, emitter)) {
        return false;
    }

    emitter << YAML::Key << "Tags" << YAML::Value << m_tags;

    return true;
}

bool TagSelector::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    if (!EntitySelector::deserialize(serializer, data)) {
        return false;
    }

    m_tags = data["Tags"].as<size_t>();

    return true;
}

void TagSelector::onEntityAdded(scene::Entity* entity) {
    if (this->isSelected(entity)) {
        m_entities.push_back(entity);
    }
}

bool TagSelector::isSelected(scene::Entity* entity) const {
    return entity->getTags() & m_tags;
}

size_t TagSelector::size() const {
    return m_entities.size();
}

scene::Entity** TagSelector::getEntities() {
    return m_entities.data();
}

void TagSelector::regenerate() {
    m_entities.clear();
    if (p_scene != nullptr) {
        for (Entity* e : p_scene->getEntities()) {
            this->onEntityAdded(e);
        }
    }
}

} // namespace scene
} // namespace aderite
