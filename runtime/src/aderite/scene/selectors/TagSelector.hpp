#pragma once

#include "aderite/scene/EntitySelector.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Entity selector where entities that match the tag list are selected
 */
class TagSelector : public EntitySelector {
public:
    /**
     * @brief Add tag to the selector
     * @param tag Tag to add
     */
    void addTag(size_t tag);

    /**
     * @brief Remove tag from the selector (this requires a full reupdate)
     * @param tag Tag to remove
     */
    void removeTag(size_t tag);

    /**
     * @brief Returns true if the entity has the specified tag, false otherwise
     */
    bool hasTag(size_t tag) const;

    /**
     * @brief Returns the tags of the selector
     */
    size_t getTags() const {
        return m_tags;
    }

    // Inherited via IEntitySelector
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
    virtual void onEntityAdded(scene::Entity* entity) override;
    virtual bool isSelected(scene::Entity* entity) override;
    virtual size_t size() const override;
    virtual scene::Entity** getEntities() override;
    virtual void regenerate() override;

private:
    size_t m_tags = 0;
    std::vector<scene::Entity*> m_entities;
};

} // namespace scene
} // namespace aderite
