#pragma once

#include "aderite/scene/EntitySelector.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Entity selector where entities that match the tag list are selected
 */
class TagSelector final : public EntitySelector {
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
    size_t getTags() const;

    // Inherited via IEntitySelector
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
    void onEntityAdded(scene::Entity* entity) override;
    bool isSelected(scene::Entity* entity) const override;
    size_t size() const override;
    scene::Entity** getEntities() override;
    void regenerate() override;

private:
    size_t m_tags = 0;
    std::vector<scene::Entity*> m_entities;
};

} // namespace scene
} // namespace aderite
