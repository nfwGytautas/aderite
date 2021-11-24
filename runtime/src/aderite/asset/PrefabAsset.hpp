#pragma once

#include "aderite/io/SerializableObject.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Asset for storing an entity that could be instantiated
 */
class PrefabAsset final : public io::SerializableObject {
public:
    ~PrefabAsset();

    /**
     * @brief Sets the prototype of the prefab
     * @param entity Entity instance
     */
    void setPrototype(scene::Entity* entity);

    /**
     * @brief Returns the prototype entity
     */
    scene::Entity* getPrototype() const;

    /**
     * @brief Instantiate a prefab in the specified scene
     * @param scene Scene to instantiate in
     * @return Entity instance
     */
    scene::Entity* instantiate(scene::Scene* scene);

    // Inherited via SerializableObject
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    scene::Entity* m_prototype = nullptr;
};

} // namespace asset
} // namespace aderite
