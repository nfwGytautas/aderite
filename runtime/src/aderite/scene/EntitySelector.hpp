#pragma once

#include <string>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Entity selector interface
 */
class EntitySelector : public io::ISerializable {
public:
    virtual ~EntitySelector() {}

    /**
     * @brief Sets the name of the selector
     * @param name New name of the selector
     */
    void setName(const std::string& name);

    /**
     * @brief Sets the scene of the selector
     * @param scene New scene of the selector
     */
    void setScene(scene::Scene* scene);

    /**
     * @brief Returns the name of the selector
     */
    const std::string& getName() const;

    /**
     * @brief Returns the scene of the selector
     */
    const scene::Scene* getScene() const;

    /**
     * @brief Function invoked when an entity was added, during this function the selector should update it's entity list
     * @param entity Entity that was added
     */
    virtual void onEntityAdded(scene::Entity* entity) = 0;

    /**
     * @brief Returns true if the specified entity is part of the selector, false otherwise
     * @param entity Entity instance
     */
    virtual bool isSelected(scene::Entity* entity) const = 0;

    /**
     * @brief Number of entities in the selector
     */
    virtual size_t size() const = 0;

    /**
     * @brief Returns the entities list of the selector
     */
    virtual scene::Entity** getEntities() = 0;

    /**
     * @brief Regenerates the entity list from scratch
     */
    virtual void regenerate() = 0;

    // Inherited from ISerializable
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

protected:
    scene::Scene* p_scene = nullptr;

private:
    std::string m_name = "";
};

} // namespace scene
} // namespace aderite
